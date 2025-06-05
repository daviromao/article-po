#include <bits/stdc++.h>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int main() {
    int N = 200; 
    int M = 30;
    int memory = 5;
    int core = 5;
    int K = N-1;

    uniform_int_distribution<int> dMi(20, 500), dCi(20, 500);

    for (int i = 0; i < N; ++i) {
        int mi = dMi(rng);
        int ci = dCi(rng);
        cout << mi << " " << ci << "\n";
    }

    cout << K << "\n";
    set<pair<int, int>> deps;
    while ((int)deps.size() < K) {
        int u = rng() % N;
        int v = rng() % N;
        if (u == v || deps.count({u, v}) || u > v) continue;
        deps.insert({u, v});
    }

    for (auto [u, v] : deps)
        cout << u + 1 << " " << v + 1 << "\n"; // 1-based
}
