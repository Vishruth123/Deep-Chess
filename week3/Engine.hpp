#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "chess.hpp"

// Week 3 — forced-mate solver built on top of Disservin's chess-library.
//
// This is NOT a general chess engine: there is no evaluation function and no
// search beyond what's needed to prove or disprove a forced mate. The chess
// library is fully responsible for legality, move generation, and notation;
// this class is responsible only for the search/proof logic.
//
// The game tree is treated as an AND/OR tree:
//   - On the mating side's ply (an OR node), success requires only ONE legal
//     move to lead to success.
//   - On the defending side's ply (an AND node), success requires EVERY
//     legal reply to lead to success.
// Short-circuiting on the first success (OR node) or first failure (AND
// node) is this search's version of alpha-beta pruning.
class Engine {
   public:
    struct MateResult {
        bool found = false;             // whether a forced mate exists within max_mate_in moves
        int mate_in = -1;                // minimum number of the mating side's own moves to force mate
        std::vector<chess::Move> pv;     // full line, root to mate (mater's move first)
        std::uint64_t nodes_visited = 0; // diagnostic: total search() calls made
    };

    // Tries mate_in = 1, 2, ..., max_mate_in in that order and returns the
    // first (and therefore minimum) value that is actually forced. Returns
    // found = false if no forced mate exists within max_mate_in moves.
    //
    // `board` is taken by value: the search works on a private copy via
    // makeMove/unmakeMove, so the caller's board is never touched.
    MateResult findMate(chess::Board board, int max_mate_in);

    // Convenience: converts a PV of Move objects into SAN strings, given the
    // board position the PV starts from. Needed because converting a Move to
    // SAN requires the board state at the moment that move is played, not the
    // final position.
    static std::vector<std::string> pvToSan(chess::Board board, const std::vector<chess::Move>& pv);

   private:
    struct TTEntry {
        bool success;
        std::vector<chess::Move> continuation;
    };

    std::uint64_t nodes_ = 0;
    std::unordered_map<std::uint64_t, TTEntry> tt_;

    // board.hash() already folds in side-to-move/castling/en-passant rights,
    // so the only extra information a transposition key needs here is the
    // remaining-ply budget: the same physical position with a different
    // amount of budget left is a different sub-problem and must not share a
    // cache entry.
    static std::uint64_t ttKey(std::uint64_t board_hash, int remaining, bool maters_turn);

    // Core recursive search. Returns true iff the side to move can force the
    // "mate" outcome within `remaining` plies. `maters_turn` says whether the
    // side to move right now is the mater (OR node) or the defender (AND
    // node). On success, `out_line` holds the move sequence from this
    // position onward (this node's own move first).
    bool search(chess::Board& board, int remaining, bool maters_turn,
                std::vector<chess::Move>& out_line);

    // Pure speed optimization, not a spec requirement (unlike Notakto's
    // mandated move order): try checks/captures/promotions first so OR-node
    // successes and AND-node refutations are found sooner.
    static void orderMoves(const chess::Board& board, chess::Movelist& moves);
};
