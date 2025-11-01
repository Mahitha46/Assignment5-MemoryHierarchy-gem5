# scripts/plot.py
# Make simple bar charts from the parsed CSV (sim_insts/sec, IPC).
# Usage:
#   python3 scripts/plot.py out.csv plots/

import sys, os, csv
import matplotlib.pyplot as plt
from collections import defaultdict

def load_rows(csv_path):
    rows = []
    with open(csv_path, "r") as f:
        reader = csv.DictReader(f)
        for r in reader:
            rows.append(r)
    return rows

def group_by(rows, key):
    d = defaultdict(list)
    for r in rows:
        d[r[key]].append(r)
    return d

def safe_float(x):
    try: return float(x)
    except: return None

def plot_ipc(rows, outdir):
    os.makedirs(outdir, exist_ok=True)
    # one bar per experiment
    labels = [f"{r['cpu']}\n{r['variant']}" for r in rows]
    values = [safe_float(r.get("system.cpu.ipc","")) or 0.0 for r in rows]

    plt.figure()
    plt.bar(labels, values)
    plt.title("IPC by CPU & Variant")
    plt.ylabel("IPC")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(os.path.join(outdir, "ipc_by_cpu_variant.png"), dpi=150)
    plt.close()

def plot_sim_seconds(rows, outdir):
    labels = [f"{r['cpu']}\n{r['variant']}" for r in rows]
    values = [safe_float(r.get("sim_seconds","")) or 0.0 for r in rows]

    plt.figure()
    plt.bar(labels, values)
    plt.title("sim_seconds by CPU & Variant")
    plt.ylabel("sim_seconds")
    plt.xticks(rotation=45, ha="right")
    plt.tight_layout()
    plt.savefig(os.path.join(outdir, "sim_seconds_by_cpu_variant.png"), dpi=150)
    plt.close()

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 scripts/plot.py <parsed_csv> <outdir>")
        sys.exit(1)
    csv_path = sys.argv[1]
    outdir = sys.argv[2]
    rows = load_rows(csv_path)
    if not rows:
        print("No rows; nothing to plot.")
        sys.exit(0)
    plot_ipc(rows, outdir)
    plot_sim_seconds(rows, outdir)
    print(f"Saved plots in {outdir}")

if __name__ == "__main__":
    main()
