/*
 * SampleDecoder.cpp
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */
#include "SampleDecoder.h"

SampleDecoder::SampleDecoder(const InputData& inputData) : inputData(inputData) {
}

SampleDecoder::~SampleDecoder() { }


using namespace std;

using Intervalo = pair<int,int>;
int encontrarInicioEOcupar(vector<Intervalo>& intervals, int duracao, int earliest) {
    for (size_t i = 0; i < intervals.size(); i++) {
        int ini = intervals[i].first;
        int fim = intervals[i].second;
        int t0 = max(ini, earliest);
        if (t0 + duracao <= fim) {
            // Ajustar intervalo ocupado
            if (ini == t0 && fim == t0 + duracao) {
                // remove todo intervalo
                intervals.erase(intervals.begin() + i);
            } else if (ini == t0) {
                intervals[i].first = t0 + duracao;
            } else if (fim == t0 + duracao) {
                intervals[i].second = t0;
            } else {
                // split interval
                intervals[i].second = t0;
                intervals.insert(intervals.begin() + i + 1, {t0 + duracao, fim});
            }
            return t0;
        }
    }
    // não achou lugar (na prática, não deveria acontecer)
    exit(1);
    return -1;
}

// Avaliar makespan (cmax)
int avaliar(const vector<Tarefa>& sol) {
    int cmax = 0;
    for (auto& t : sol) {
        cmax = max(cmax, t.start[2] + t.mi);
    }
    return cmax;
}


const int alphaPenalizacao = 1e3; // Penalização para precedências não respeitadas

// Runs in \Theta(n \log n):
double SampleDecoder::decode(const std::vector< double >& chromosome) const {
    double penalizacao = 0;
    vector<pair<double,int>> prioridade(this->inputData.N);
    for (int i = 0; i < this->inputData.N; i++) {
        prioridade[i] = {chromosome[i], i};
    }
    sort(prioridade.begin(), prioridade.end());
    vector<int> ordem;
    for (auto& p : prioridade) ordem.push_back(p.second);

    vector<Tarefa> sol(this->inputData.N);
    for (int i = 0; i < this->inputData.N; i++) {
        int base = this->inputData.N + 4*i;
        sol[i].id = i;
        sol[i].mi = this->inputData.mi[i];
        sol[i].ci = this->inputData.ci[i];
        sol[i].npu = min((int)(chromosome[base + 0] * this->inputData.M), this->inputData.M-1);
        sol[i].recurso = {
            min((int)(chromosome[base + 1] * this->inputData.memory), this->inputData.memory-1),
            min((int)(chromosome[base + 2] * this->inputData.core), this->inputData.core-1),
            min((int)(chromosome[base + 3] * this->inputData.memory), this->inputData.memory-1)
        };
        sol[i].start = vector<int>(3, 0);
    }

    vector<vector<vector<Intervalo>>> livreMem(this->inputData.M, vector<vector<Intervalo>>(this->inputData.memory, {{0, 1000000}}));
    vector<vector<vector<Intervalo>>> livreCPU(this->inputData.M, vector<vector<Intervalo>>(this->inputData.core, {{0, 1000000}}));

    vector<int> maxPredecessorEnd(this->inputData.N, 0);
    for (int u = 0; u < this->inputData.N; u++) {
        for (int pre : this->inputData.prec[u]) {
            maxPredecessorEnd[u] = 0;
        }
    }
    
    // verifica ordem se as precedências são respeitadas
    vector<bool> visitado(this->inputData.N, false);
    for (int idx : ordem) {
        Tarefa& t = sol[idx];
        for (int pre : this->inputData.prec[t.id]) {
            if (visitado[pre]) {
                maxPredecessorEnd[t.id] = max(maxPredecessorEnd[t.id], sol[pre].start[2] + sol[pre].mi);
            }else {
                penalizacao += alphaPenalizacao; // penaliza se a tarefa não foi visitada antes
            }
        }
        visitado[t.id] = true;
    }

    for (int idx : ordem) {
        Tarefa& t = sol[idx];

        int inicio = 0;
        for (int pre : this->inputData.prec[t.id]) {
            int fimPre = sol[pre].start[2] + sol[pre].mi;
            if (fimPre > inicio) inicio = fimPre;
        }

        int t0 = encontrarInicioEOcupar(livreMem[t.npu][t.recurso[0]], t.mi, inicio);
        int t1 = encontrarInicioEOcupar(livreCPU[t.npu][t.recurso[1]], t.ci, t0 + t.mi);
        int t2 = encontrarInicioEOcupar(livreMem[t.npu][t.recurso[2]], t.mi, t1 + t.ci);

        t.start = {t0, t1, t2};
    }

    return avaliar(sol) + penalizacao;
}
