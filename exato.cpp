#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
#define IloBigM 1000000000
/*
Dado um grafo G = (V , E), determinar a clique máxima de G, ou seja, o maior
subgrafo completo em G
*/

int main() {
    int N = 0, M = 0, memory=0, core=0;
    cin>> N >> M >> memory >> core;

    vector<int> mi(N), ci(N);
    for(int i = 0; i < N; i++){
        cin >> mi[i] >> ci[i];
    }

    int K;
    cin >> K;
    vector<pair<int, int>> prec;
    for(int i = 0; i < K; i++){
        int u, v;
        cin >> u >> v;
        u--; v--;
        prec.push_back({u, v});
    }

    IloEnv env;
    try {
        IloModel model(env);

        
        // Variáveis
        IloArray<IloIntVarArray> t(env, N); // t[i][step]
        IloIntVarArray p(env, N, 0, M - 1);  // NPU
        IloArray<IloIntVarArray> r(env, N); // r[i][step] nucleo
        IloIntVar cmax(env);

        for (int i = 0; i < N; i++) {
            t[i] = IloIntVarArray(env, 3, 0, IloIntMax);
            r[i] = IloIntVarArray(env, 3);

            // Os passos 1, 2, 3
            r[i][0] = IloIntVar(env, 0, memory - 1); 
            r[i][1] = IloIntVar(env, 0, core - 1); 
            r[i][2] = IloIntVar(env, 0, memory - 1); 
        }

        // Objetivo
        model.add(IloMinimize(env, cmax));

        // Restrições
        // Ordem dos steps
        for(int i = 0; i < N; i++){
            model.add(t[i][1] >= t[i][0] + mi[i]);
            model.add(t[i][2] >= t[i][1] + ci[i]);
        }

        // Precendência entre tarefas
        // u -> v: significa que v depende de u
        for(auto [u, v] : prec){
            model.add(t[v][0] >= t[u][2]+mi[u]);
        }

        // Tempo maximo
        for (int i = 0; i < N; ++i)
            model.add(cmax >= t[i][2] + mi[i]);

        // Sobreposição de tarefas
        for (int i = 0; i < N; ++i) {
            for (int j = i + 1; j < N; ++j) {
                for (int s1 : {0, 1, 2}) {
                    for (int s2 : {0, 1, 2}) {
                        bool mem1 = (s1 != 1), mem2 = (s2 != 1);
                        if (mem1 != mem2) continue;

                        IloInt dur_i = (s1 == 1 ? ci[i] : mi[i]);
                        IloInt dur_j = (s2 == 1 ? ci[j] : mi[j]);
            
                        // Verificar mesma NPU e mesmo core
                        IloExpr sameCore(env);
                        sameCore = (p[i] == p[j] && r[i][s1]==r[j][s2]);
            
                        // Linearizar a ordem
                        IloBoolVar before(env);
            
                        // Adicionando a restrição linear para a ordem das tarefas:
                        // Caso before = 1, t[i][s] + dur_i <= t[j][s]
                        // Caso before = 0, t[j][s] + dur_j <= t[i][s]
                        model.add(t[i][s1] + dur_i <= t[j][s2] + IloBigM * (1 - before) + IloBigM * (1 - sameCore));
                        model.add(t[j][s2] + dur_j <= t[i][s1] + IloBigM * (before) + IloBigM * (1 - sameCore));
                    }
                }
            }
        }
        
        
        // Solução
        IloCplex cplex(model);
        cplex.extract(model); 
        cplex.setOut(env.getNullStream());
        cplex.setWarning(env.getNullStream());
        cplex.setError(env.getNullStream());
        cplex.setParam(IloCplex::Param::MIP::Display, 0);
        if (cplex.solve()) {
            cout << cplex.getObjValue() << endl;
            // vector<tuple<int, int, int, int, int>> executionOrder;

            // for (int i = 0; i < N; i++) {
            //     for (int s = 0; s < 3; s++) {
            //         int tj = cplex.getValue(t[i][s]);
            //         int oj = i + 1;
            //         int sj = s + 1; 
            //         int pj = cplex.getValue(p[i]);
            //         int rj = cplex.getValue(r[i][s]);
            //         executionOrder.push_back(make_tuple(tj, oj, sj, pj + 1, rj + 1));
            //     }
            // }

            // sort(executionOrder.begin(), executionOrder.end());
            
            // // Imprimir os detalhes das execuções
            // for (auto& exec : executionOrder) {
            //     cout << get<0>(exec) << " " << get<1>(exec) << " " << get<2>(exec) << " "
            //          << get<3>(exec) << " " << get<4>(exec) << endl;
            // }
        } else {
            cout << "Sem solução viável." << endl;
        }
    } catch (IloException& e) {
        cerr << "Erro: " << e.getMessage() << endl;
    }
    env.end();
    return 0;
}
