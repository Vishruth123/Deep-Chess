#pragma once

#include "chess.hpp"

class Engine {
public:
    Engine() = default;

    chess::Move findBestMove(chess::Board board, int depth);

private:
    int minimax(
    chess::Board& board,
    int depth,
    int alpha,
    int beta
    );

    int evaluate(const chess::Board& board);
};