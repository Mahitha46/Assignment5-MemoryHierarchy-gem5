# scripts/parse_stats.py
# Parse Gem5 stats.txt files into a single CSV for easy plotting.
# Usage:
#   python3 scripts/parse_stats.py results/ out.csv

import sys, os, re, csv, glob

# Choose a set of common metrics (missing keys will be left blank)
METRICS = [
    "sim_seconds",
    "sim_insts",
    "sim_ticks",
    "host_seconds",
    "host_inst_rate",
    "system.cpu.cpi",
    "system.cpu.ipc",
    "system.cpu.numCycles",
    "system.l2.overall_misses::total",
    "system.l2.overall_hits::total",
    "system.l1d.overall_misses::total",
    "system.l1d.overall_hits::total",
]

def parse_stats_file(path):
    data = {}
    with open(path, "r", errors="ignore") as f:
        for line in f:
            line=line.strip()
            if not line or line.startswith("#"): 
                continue
            parts = line.split()
            # Expected form: "<name>    <value>    # comment"
            if len(parts) >= 2:
                key = parts[0]
                try:
                    val = float(parts[1])
                except ValueError:
                    continue
                data[key] = val
    return data

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 scripts/parse_stats.py <results_dir> <out_csv>")
        sys.exit(1)
    root = sys.argv[1]
    out_csv = sys.argv[2]
    rows = []
    for stats_path in glob.glob(os.path.join(root, "*", "stats.txt")):
        exp = os.path.basename(os.path.dirname(stats_path))  # e.g., DerivO3CPU_vecadd_avx
        parts = exp.split("_", 1)
        cpu, variant = (parts[0], parts[1]) if len(parts)==2 else (exp, "")
        d = parse_stats_file(stats_path)
        row = {"exp":exp, "cpu":cpu, "variant":variant, "stats_path":stats_path}
        for m in METRICS:
            row[m] = d.get(m, "")
        rows.append(row)

    # write CSV
    with open(out_csv, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=["exp","cpu","variant","stats_path"]+METRICS)
        w.writeheader()
        for r in rows:
            w.writerow(r)
    print(f"Wrote {out_csv} with {len(rows)} rows.")

if __name__ == "__main__":
    main()
