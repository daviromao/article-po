import subprocess
from datetime import datetime
id = datetime.now().strftime("yes%Y%m%d_%H%M%S_yes")

subprocess.run(["mkdir", id])
out = ["entrada,exato,heuristico_10,heuristico_100,heuristico_1000,brkga, time_exato,time_heuristico_10,time_heuristico_100,time_heuristico_1000,time_brkga\n"]

for n in [10, 20, 30, 40, 50]:
    subprocess.run(["mkdir", f"./{id}/n{n}"])
    for i in range(10):
        entrada = f"./{id}/n{n}/instancia_{i:02d}.txt"

        with open(entrada, "w") as f:
            subprocess.run(["./gen", str(n)], stdout=f)

        with open(entrada, "r") as f:
            start_time = datetime.now()
            saida_exato = -1
            if(n<60):
                saida_exato = subprocess.run(["./exato"], stdin=f, capture_output=True, text=True)
            time_exato = datetime.now() - start_time
        
        with open(entrada, "r") as f:
            print("Iniciando heurístico com 10 iterações...")
            start_time = datetime.now()
            saida_heur_10 = subprocess.run(["./heuristico", "10"], stdin=f, capture_output=True, text=True)
            time_heur_10 = datetime.now() - start_time

        with open(entrada, "r") as f:
            print("Iniciando heurístico com 100 iterações...")
            start_time = datetime.now()
            saida_heur_100 = subprocess.run(["./heuristico", "100"], stdin=f, capture_output=True, text=True)
            time_heur_100 = datetime.now() - start_time
        
        with open(entrada, "r") as f:
            print("Iniciando heurístico com 1000 iterações...")
            start_time = datetime.now()
            saida_heur_1000 = subprocess.run(["./heuristico", "1000"], stdin=f, capture_output=True, text=True)
            time_heur_1000 = datetime.now() - start_time

        #brkga
        with open(entrada, "r") as f:
            print("Iniciando BRKGA...")
            start_time = datetime.now()
            saida_brkga = subprocess.run(["./brkga"], stdin=f, capture_output=True, text=True)
            time_brkga = datetime.now() - start_time

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

        
        try:
            val_brkga = int(saida_brkga.stdout.strip().splitlines()[0])
        except:
            val_brkga = "ERRO"

        print(f"Instância {i:02d}: exato = {val_exato}, heurístico 10 = {val_heur_10}, heurístico 100 = {val_heur_100}, heurístico 1000 = {val_heur_1000}, BRKGA = {val_brkga}")

        out.append(f"{entrada},{val_exato},{val_heur_10},{val_heur_100},{val_heur_1000},{val_brkga},{time_exato.total_seconds()},{time_heur_10.total_seconds()},{time_heur_100.total_seconds()},{time_heur_1000.total_seconds()},{time_brkga.total_seconds()}\n")
    with open(f"./{id}/n{n}/relatorio.csv", "w") as f:
        f.writelines(out)
