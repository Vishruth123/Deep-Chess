#include <iostream>
#include <chrono>
#include "Engine.hpp"

using namespace chess;

void run(const std::string& label, const std::string& fen, int max_n) {
    Engine engine;
    auto t0 = std::chrono::high_resolution_clock::now();
    auto res = engine.findMate(Board(fen), max_n);
    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << label << " | found=" << res.found;
    if (res.found) {
        std::cout << " mate_in=" << res.mate_in;
        auto sans = Engine::pvToSan(Board(fen), res.pv);
        std::cout << " line=";
        for (auto& s : sans) std::cout << s << " ";
    }
    std::cout << " nodes=" << res.nodes_visited << " time_ms=" << ms << "\n";
}

int main() {
    // 1. Trivial mate-in-1 (back rank).
    run("mate-in-1 sanity", "6k1/5ppp/8/8/8/8/5PPP/4R1K1 w - - 0 1", 1);

    // 2. Starting position should find nothing within a small budget.
    run("startpos (expect not found)", constants::STARTPOS, 3);

    // 3. A clean, verified mate_in_3.json entry.
    run("mate_in_3.json sample", "r1b1kb1r/pppp1ppp/5q2/4n3/3KP3/2N3PN/PPP4P/R1BQ1B1R b kq - 0 1", 3);

    // 4. A clean, verified mate_in_4.json entry.
    run("mate_in_4.json sample", "r5rk/2p1Nppp/3p3P/pp2p1P1/4P3/2qnPQK1/8/R6R w - - 1 0", 4);

    // 5. One of the *truncated* mate_in_2.json entries (dataset only records
    //    "Rb1" as the solution, which we already proved isn't checkmate).
    //    Does the position genuinely have a complete mate in 2 anyway?
    run("mate_in_2.json TRUNCATED entry (Rb1)", "k7/p1K2n2/p7/3p1r2/8/8/8/2R5 w - - 0 1", 2);

    return 0;
}
