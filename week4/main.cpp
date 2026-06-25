#include <iostream>
#include <sstream>
#include <string>

#include "Engine.hpp"

using namespace chess;

int main() {

    Engine engine;
    Board board;

    std::string line;

    while (std::getline(std::cin, line)) {

        std::istringstream iss(line);

        std::string command;
        iss >> command;

        if (command == "uci") {

            std::cout << "id name Week4Engine\n";
            std::cout << "id author Vishruth\n";
            std::cout << "uciok\n";

        }

        else if (command == "isready") {

            std::cout << "readyok\n";

        }

        else if (command == "position") {

            std::string token;
            iss >> token;

            if (token == "startpos") {

                board = Board();

            }

            else if (token == "fen") {

                std::string fen;

                for (int i = 0; i < 6; i++) {

                    std::string part;
                    iss >> part;

                    if (i)
                        fen += " ";

                    fen += part;
                }

                board = Board(fen);
            }

            std::string word;

            if (iss >> word && word == "moves") {

                std::string moveString;

                while (iss >> moveString) {

                    Move move = uci::uciToMove(board, moveString);

                    board.makeMove(move);
                }
            }
        }

        else if (command == "go") {

            std::string token;
            iss >> token;

            int depth = 5;

            if (token == "depth")
                iss >> depth;

            Move bestMove = engine.findBestMove(board, depth);

            std::cout << "bestmove "
                    << uci::moveToUci(bestMove)
                    << std::endl;
        }

        else if (command == "quit") {

            break;

        }
    }

    return 0;
}