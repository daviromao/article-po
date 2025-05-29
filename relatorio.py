import subprocess
from datetime import datetime
id = datetime.now().strftime("%Y%m%d_%H%M%S")

subprocess.run(["mkdir", id])
out = ["entrada,exato,heuristico_10,heuristico_100,heuristico_1000,time_exato,time_heuristico_10,time_heuristico_100,time_heuristico_1000\n"]
for i in range(30):
    entrada = f"{id}/in{i}.txt"

    with open(entrada, "w") as f:
        subprocess.run(["./gen"], stdout=f)

    with open(entrada, "r") as f:
        start_time = datetime.now()
        saida_exato = subprocess.run(["./exato"], stdin=f, capture_output=True, text=True)
        time_exato = datetime.now() - start_time

    with open(entrada, "r") as f:
        start_time = datetime.now()
        saida_heur_10 = subprocess.run(["./heuristico", "10"], stdin=f, capture_output=True, text=True)
        time_heur_10 = datetime.now() - start_time

    with open(entrada, "r") as f:
        start_time = datetime.now()
        saida_heur_100 = subprocess.run(["./heuristico", "100"], stdin=f, capture_output=True, text=True)
        time_heur_100 = datetime.now() - start_time
    
    with open(entrada, "r") as f:
        start_time = datetime.now()
        saida_heur_1000 = subprocess.run(["./heuristico", "1000"], stdin=f, capture_output=True, text=True)
        time_heur_1000 = datetime.now() - start_time

    try:
        val_exato = int(saida_exato.stdout.strip().splitlines()[0])
    except:
        val_exato = "ERRO"


    try:
        val_heur_10 = int(saida_heur_10.stdout.strip().splitlines()[0])
    except:
        val_heur_10 = "ERRO"
    
    try:
        val_heur_100 = int(saida_heur_100.stdout.strip().splitlines()[0])
    except:
        val_heur_100 = "ERRO"

    try:
        val_heur_1000 = int(saida_heur_1000.stdout.strip().splitlines()[0])
    except:
        val_heur_1000 = "ERRO"

    print(f"Instância {i:02d}: exato = {val_exato}, heurístico 10 = {val_heur_10}, heurístico 100 = {val_heur_100}, heurístico 1000 = {val_heur_1000}")

    out.append(f"{entrada},{val_exato},{val_heur_10},{val_heur_100},{val_heur_1000},{time_exato.total_seconds()},{time_heur_10.total_seconds()},{time_heur_100.total_seconds()},{time_heur_1000.total_seconds()}\n")

with open(f"{id}/relatorio.csv", "w") as f:
    f.writelines(out)