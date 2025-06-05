import matplotlib.pyplot as plt

with open("out") as f:
    linhas = [line.strip().split() for line in f if line.strip()]
linhas = linhas[1:]
tarefas = []

for t, tid, step, npu, rec in linhas:
    tarefas.append({
        "start": int(t),
        "dur": None,
        "id": int(tid),
        "step": int(step),
        "npu": int(npu),
        "rec": int(rec)
    })

with open("in") as f:
    N, M, memory, core = map(int, f.readline().split())
    mi_ci = {}
    for i in range(N):
        mi, ci = map(int, f.readline().split())
        mi_ci[i + 1] = (mi, ci)

for t in tarefas:
    mi, ci = mi_ci[t["id"]]
    t["dur"] = mi if t["step"] in [1, 3] else ci

def posicao_y(npu, step, rec):
    if step in [1, 3]:
        return (npu - 1) * (memory + core) + (rec - 1)
    else:
        return (npu - 1) * (memory + core) + memory + (rec - 1)

cores = ['#ff9999', '#99ccff', '#99ff99']

fig, ax = plt.subplots(figsize=(12, 6))

for t in tarefas:
    y = posicao_y(t["npu"], t["step"], t["rec"])
    ax.broken_barh(
        [(t["start"], t["dur"])],
        (y, 0.8),
        facecolors=cores[t["step"] - 1],
        edgecolor='black'
    )
    ax.text(
        t["start"] + t["dur"] / 2,
        y + 0.4,
        f"op {t['id']}\nstep {t['step']}",
        ha='center',
        va='center',
        fontsize=7,
        color='black'
    )

yticks = []
ylabels = []
for npu in range(1, M + 1):
    base = (npu - 1) * (memory + core)
    for i in range(memory):
        yticks.append(base + i + 0.4)
        ylabels.append(f"NPU {npu} - Memory Core {i + 1}")
    for i in range(core):
        yticks.append(base + memory + i + 0.4)
        ylabels.append(f"NPU {npu} - Computation Core {i + 1}")

ax.set_yticks(yticks)
ax.set_yticklabels(ylabels)
ax.set_xlabel("Seconds")
ax.set_ylim(-0.5, M * (memory + core))
ax.set_xlim(left=0)
ax.set_title("Task Scheduling")
ax.grid(axis='x', linestyle=':', color='gray')

plt.tight_layout()
plt.savefig("grafico.png", dpi=300)