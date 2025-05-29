#include <bits/stdc++.h>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int main() {
    int N = rng() % 50 + 2; 
    int M = rng() % 8 + 2;
    int memory = rng() % 4 + 1;
    int core = rng() % 4 + 1;
    int K = min(rng() % (N*(N-1))/2 + 1, (ulong) (N*(N-1))/2);
    K = min(K, 500);
    cout << N << " " << M << " " << memory << " " << core << "\n";

    uniform_int_distribution<int> dMi(1, 10), dCi(1, 10);

    
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
