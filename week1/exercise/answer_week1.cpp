#include <bits/stdc++.h>
using namespace std;

map<pair<int,int>, pair<long long,long long>> memo;

pair<long long, long long> rec(int idx, int turn, vector<long long>& a, int n) {
    if (idx >= n) return {0, 0};

    auto key = make_pair(idx, turn);
    if (memo.count(key)) return memo[key];

    // Option 1: take 1 number
    long long take1 = a[idx];
    auto [p1_a, p2_a] = rec(idx + 1, 1 - turn, a, n);

    long long opt1_p1 = p1_a, opt1_p2 = p2_a;
    if (turn == 0) opt1_p1 += take1; else opt1_p2 += take1;

    pair<long long,long long> best = {opt1_p1, opt1_p2};

    // Option 2: take 2 numbers (if possible)
    if (idx + 1 < n) {
        long long take2 = a[idx] + a[idx+1];
        auto [p1_b, p2_b] = rec(idx + 2, 1 - turn, a, n);

        long long opt2_p1 = p1_b, opt2_p2 = p2_b;
        if (turn == 0) opt2_p1 += take2; else opt2_p2 += take2;

        // current player picks whichever option maximizes THEIR score
        if (turn == 0) {
            if (opt2_p1 > best.first) best = {opt2_p1, opt2_p2};
        } else {
            if (opt2_p2 > best.second) best = {opt2_p1, opt2_p2};
        }
    }

    memo[key] = best;
    return best;
}

string solve(int n, vector<long long> a_int) {
    memo.clear();
    vector<long long> a(a_int.begin(), a_int.end());
    auto [p1, p2] = rec(0, 0, a, n);
    if (p1 > p2) return "Player 1";
    if (p2 > p1) return "Player 2";
    return "Draw";
}