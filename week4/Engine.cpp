#include "Engine.hpp"

#include <algorithm>
#include <limits>

using namespace chess;

const int PAWN_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int KNIGHT_TABLE[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int BISHOP_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int ROOK_TABLE[64] = {
     0,0,0,5,5,0,0,0,
    -5,0,0,0,0,0,0,-5,
    -5,0,0,0,0,0,0,-5,
    -5,0,0,0,0,0,0,-5,
    -5,0,0,0,0,0,0,-5,
    -5,0,0,0,0,0,0,-5,
     5,10,10,10,10,10,10,5,
     0,0,0,0,0,0,0,0
};

const int QUEEN_TABLE[64] = {
    -20,-10,-10,-5,-5,-10,-10,-20,
    -10,0,0,0,0,0,0,-10,
    -10,0,5,5,5,5,0,-10,
    -5,0,5,5,5,5,0,-5,
     0,0,5,5,5,5,0,-5,
    -10,5,5,5,5,5,0,-10,
    -10,0,5,0,0,0,0,-10,
    -20,-10,-10,-5,-5,-10,-10,-20
};

const int KING_TABLE[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20,20,0,0,0,0,20,20,
     20,30,10,0,0,10,30,20
};

int Engine::evaluate(const Board& board) {
    int score = 0;

    

    score += 100 * board.pieces(PieceType::PAWN,   Color::WHITE).count();
    score -= 100 * board.pieces(PieceType::PAWN,   Color::BLACK).count();

    score += 320 * board.pieces(PieceType::KNIGHT, Color::WHITE).count();
    score -= 320 * board.pieces(PieceType::KNIGHT, Color::BLACK).count();

    score += 330 * board.pieces(PieceType::BISHOP, Color::WHITE).count();
    score -= 330 * board.pieces(PieceType::BISHOP, Color::BLACK).count();

    score += 500 * board.pieces(PieceType::ROOK,   Color::WHITE).count();
    score -= 500 * board.pieces(PieceType::ROOK,   Color::BLACK).count();

    score += 900 * board.pieces(PieceType::QUEEN,  Color::WHITE).count();
    score -= 900 * board.pieces(PieceType::QUEEN,  Color::BLACK).count();

    return score;
}

Move Engine::findBestMove(Board board, int depth) {
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (moves.empty()) {
        return Move::NO_MOVE;
    }

    Move best_move = moves[0];
    int best_score;

    if (board.sideToMove() == Color::WHITE) {
        best_score = std::numeric_limits<int>::min();

        for (const Move& move : moves) {
            board.makeMove(move);

            int score = minimax(board, depth - 1,
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());

            board.unmakeMove(move);

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }
    }
    else {
        best_score = std::numeric_limits<int>::max();

        for (const Move& move : moves) {
            board.makeMove(move);

            int score = minimax(board, depth - 1,
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());

            board.unmakeMove(move);

            if (score < best_score) {
                best_score = score;
                best_move = move;
            }
        }
    }

    return best_move;
}

int Engine::minimax(Board& board, int depth, int alpha, int beta) {

    auto [reason, result] = board.isGameOver();

    if (result != GameResult::NONE) {

        if (reason == GameResultReason::CHECKMATE) {

            if (board.sideToMove() == Color::WHITE)
                return -100000;

            return 100000;
        }

        return 0;
    }

    if (depth == 0)
        return evaluate(board);

    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::ALL>(moves, board);

    if (board.sideToMove() == Color::WHITE) {

        int best = std::numeric_limits<int>::min();

        for (const Move& move : moves) {

            board.makeMove(move);

            int score = minimax(board, depth - 1, alpha, beta);

            board.unmakeMove(move);

            best = std::max(best, score);
            alpha = std::max(alpha, best);

            if (beta <= alpha)
                break;
        }

        return best;
    }

    else {

        int best = std::numeric_limits<int>::max();

        for (const Move& move : moves) {

            board.makeMove(move);

            int score = minimax(board, depth - 1, alpha, beta);

            board.unmakeMove(move);

            best = std::min(best, score);
            beta = std::min(beta, best);

            if (beta <= alpha)
                break;
        }

        return best;
    }
}