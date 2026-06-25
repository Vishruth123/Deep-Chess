#include <iostream>
#include <string>

#include "Engine.hpp"

using namespace chess;

int main() {
    std::string fen;
    int max_mate_in;

    std::cout << "Enter FEN: ";
    std::getline(std::cin, fen);

    std::cout << "Maximum mate depth: ";
    std::cin >> max_mate_in;

    Board board = Board::fromFen(fen);

    Engine engine;

    Engine::MateResult result = engine.findMate(board, max_mate_in);

    if (!result.found) {
        std::cout << "\nNo forced mate found within "
                  << max_mate_in
                  << " moves.\n";
        return 0;
    }

    std::cout << "\nForced mate found!\n";
    std::cout << "Mate in " << result.mate_in << "\n";
    std::cout << "Nodes visited: " << result.nodes_visited << "\n";

    auto san_moves = Engine::pvToSan(board, result.pv);

    std::cout << "\nPrincipal Variation:\n";

    for (const auto& move : san_moves) {
        std::cout << move << "\n";
    }

    return 0;

}