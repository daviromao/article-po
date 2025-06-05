#include <bits/stdc++.h>
using namespace std;
using Intervalo = pair<int, int>;

struct Tarefa {
    int id, mi, ci;
    vector<int> start; 
    int npu;
    vector<int> recurso;
};

int N, M, memory, core, K;
vector<Tarefa> tarefas;
vector<pair<int, int>> precedencias;
vector<vector<int>> preced;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

vector<int> topo_sort() {
    vector<vector<int>> adj(N);
    vector<int> in_deg(N, 0);

    for (auto [u, v] : precedencias) {
        adj[u].push_back(v);
        in_deg[v]++;
    }

    queue<int> q;
    for (int i = 0; i < N; i++) if (in_deg[i] == 0) q.push(i);

    vector<int> ordem;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        ordem.push_back(u);
        for (int v : adj[u]) {
            if (--in_deg[v] == 0)
                q.push(v);
        }
    }
    return ordem;
}

int avaliar(const vector<Tarefa>& sol) {
    int cmax = 0;
    for (auto& t : sol)
        cmax = max(cmax, t.start[2] + t.mi);
    return cmax;
}

int encontrarInicioEOcupar(set<Intervalo>& s, int duracao, int earliest) {
    for (auto it = s.begin(); it != s.end(); ++it) {
        int ini = it->first, fim = it->second;
        int t0 = max(ini, earliest);
        if (t0 + duracao <= fim) {
            it = s.erase(it);
            if (ini < t0) s.insert({ini, t0});
            if (t0 + duracao < fim) s.insert({t0 + duracao, fim});

            return t0;
        }
    }

    cout << "em tese, nao deveria chegar aqui" << endl;
    exit(1);
}

vector<Tarefa> gerar_solucao() {
    vector<Tarefa> sol(N);
    vector<bool> preenchido(N, false);
    vector<int> ordem = topo_sort();

    map<pair<int, int>, set<Intervalo>> livreMem; 
    map<pair<int, int>, set<Intervalo>> livreCPU; 

    for (int npu = 0; npu < M; npu++) {
        for (int mem = 0; mem < memory; mem++) {
            livreMem[{npu, mem}].insert({0, INT_MAX});
        }
        for (int cor = 0; cor < core; cor++) {
            livreCPU[{npu, cor}].insert({0, INT_MAX});
        }
    }

    uniform_int_distribution<int> memDist(0, memory - 1);
    uniform_int_distribution<int> coreDist(0, core - 1);

    for (int i : ordem) {
        Tarefa t;
        t.id = i;
        t.mi = tarefas[i].mi;
        t.ci = tarefas[i].ci;
        t.npu = rng() % M;

        t.recurso = {memDist(rng), coreDist(rng), memDist(rng)};

        t.start = {0, 0, 0};

        int inicio = 0;
        for (auto u : preced[i]) {
            auto& ant = sol[u];
            inicio = max(inicio, ant.start[2] + ant.mi);
        }

        auto& intervalsM1 = livreMem[{t.npu, t.recurso[0]}];
        auto& intervalsC  = livreCPU[{t.npu, t.recurso[1]}];
        auto& intervalsM2  = livreMem[{t.npu, t.recurso[2]}];

        int t0 = encontrarInicioEOcupar(intervalsM1, t.mi, inicio);
        int t1 = encontrarInicioEOcupar(intervalsC, t.ci, t0 + t.mi);
        int t2 = encontrarInicioEOcupar(intervalsM2, t.mi, t1 + t.ci);

        t.start = {t0, t1, t2};
        sol[i] = t;
        preenchido[i] = true;
    }

    return sol;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <inters>\n";
        return 1;
    }

    int inters = atoi(argv[1]);
    
    cin >> N >> M >> memory >> core;
    tarefas.resize(N);
    for (int i = 0; i < N; i++) {
        tarefas[i].id = i;
        cin >> tarefas[i].mi >> tarefas[i].ci;
    }

    cin >> K;
    precedencias.resize(K);
    preced.resize(N);
    for (int i = 0; i < K; i++) {
        int u, v;
        cin >> u >> v;
        precedencias[i] = {u - 1, v - 1};
        preced[v - 1].push_back(u - 1);
    }

    int best = INT_MAX;
    vector<Tarefa> melhor;

    for (int it = 0; it < inters; it++) {
        auto sol = gerar_solucao();
        // removemos o passo da busca local
        int cmax = avaliar(sol);
        if (cmax < best) {
            best = cmax;
            melhor = sol;
        }
    }

    cout << best << endl;
    for (auto& t : melhor) {
        for (int s = 0; s < 3; s++) {
            cout << t.start[s] << " " << t.id + 1 << " " << s + 1 << " "
                 << t.npu + 1 << " " << t.recurso[s] + 1 << "\n";
        }
    }
}
