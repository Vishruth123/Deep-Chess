#include "Engine.hpp"

#include <algorithm>

using namespace chess;

std::uint64_t Engine::ttKey(std::uint64_t board_hash, int remaining, bool maters_turn) {
    std::uint64_t key = board_hash;
    key = key * 1000003ull + static_cast<std::uint64_t>(remaining) * 2ull +
          (maters_turn ? 1ull : 0ull);
    return key;
}

void Engine::orderMoves(const Board& board, Movelist& moves) {
    std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) {
        auto score = [&board](const Move& m) {
            int s = 0;
            if (board.isCapture(m)) s += 2;
            if (m.typeOf() == Move::PROMOTION) s += 2;
            return s;
        };
        return score(a) > score(b);
    });
}

bool Engine::search(Board& board, int remaining, bool maters_turn, std::vector<Move>& out_line) {
    ++nodes_;

    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        // No legal moves: either checkmate or stalemate right now. Being
        // mated is a success only if the one mated is the defender (i.e. the
        // mater's previous move delivered it). Stalemate is always a
        // failure -- nobody is forced into mate by a draw.
        out_line.clear();
        return board.inCheck() && !maters_turn;
    }

    if (remaining == 0) {
        // Budget exhausted with the game still going: mate wasn't forced in
        // time along this line.
        return false;
    }

    const std::uint64_t key = ttKey(board.hash(), remaining, maters_turn);
    auto cached = tt_.find(key);
    if (cached != tt_.end()) {
        out_line = cached->second.continuation;
        return cached->second.success;
    }

    orderMoves(board, moves);

    bool result;
    std::vector<Move> best_line;

    if (maters_turn) {
        // OR node: one success is enough.
        result = false;
        for (const auto& m : moves) {
            board.makeMove(m);
            std::vector<Move> sub;
            bool ok = search(board, remaining - 1, false, sub);
            board.unmakeMove(m);
            if (ok) {
                best_line = {m};
                best_line.insert(best_line.end(), sub.begin(), sub.end());
                result = true;
                break;
            }
        }
    } else {
        // AND node: every reply must lead to success.
        result = true;
        bool have_line = false;
        for (const auto& m : moves) {
            board.makeMove(m);
            std::vector<Move> sub;
            bool ok = search(board, remaining - 1, true, sub);
            board.unmakeMove(m);
            if (!ok) {
                result = false;
                break;
            }
            if (!have_line) {
                best_line = {m};
                best_line.insert(best_line.end(), sub.begin(), sub.end());
                have_line = true;
            }
        }
        if (!result) best_line.clear();
    }

    tt_[key] = TTEntry{result, best_line};
    out_line = best_line;
    return result;
}

Engine::MateResult Engine::findMate(Board board, int max_mate_in) {
    nodes_ = 0;
    tt_.clear();

    for (int n = 1; n <= max_mate_in; ++n) {
        const int budget = 2 * n - 1;
        std::vector<Move> line;
        // search() always restores `board` to its pre-call state (every
        // makeMove is paired with an unmakeMove before the function
        // returns), so the same `board` object is safe to reuse across
        // iterations.
        if (search(board, budget, /*maters_turn=*/true, line)) {
            return MateResult{true, n, line, nodes_};
        }
    }

    return MateResult{false, -1, {}, nodes_};
}

std::vector<std::string> Engine::pvToSan(Board board, const std::vector<Move>& pv) {
    std::vector<std::string> result;
    result.reserve(pv.size());
    for (const auto& m : pv) {
        result.push_back(uci::moveToSan(board, m));
        board.makeMove(m);
    }
    return result;
}
