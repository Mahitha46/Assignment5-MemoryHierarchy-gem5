# DLP_Gem5_Simulation_Package

This package compiles three `vecadd` binaries (scalar/SSE2/AVX2) and runs them under Gem5 (X86, SE mode) across three CPU models (TimingSimpleCPU, MinorCPU, DerivO3CPU). It also parses `stats.txt` into CSV and plots IPC and sim_seconds.

## Folder layout
```
DLP_Gem5_Simulation_Package/
├── Makefile
├── src/
│   ├── vecadd_scalar.c
│   ├── vecadd_sse.c
│   └── vecadd_avx.c
├── scripts/
│   ├── run_gem5.sh
│   ├── run_all.sh
│   ├── parse_stats.py
│   └── plot.py
├── results/              # Gem5 outputs land here (auto-created)
└── plots/                # Charts saved here
```

## 1) Build the vecadd binaries (WSL)
```bash
cd /mnt/c/Users/dkuma/Downloads/DLP_Gem5_Simulation_Package
dos2unix scripts/*.sh || true
make clean && make
# Binaries appear in ./bin : vecadd_scalar, vecadd_sse, vecadd_avx
```

If you prefer manual GCC commands:
```bash
mkdir -p bin
gcc -O3 -fno-tree-vectorize -march=x86-64 -mtune=native src/vecadd_scalar.c -o bin/vecadd_scalar
gcc -O3 -msse2 -mfpmath=sse -march=x86-64 -mtune=native src/vecadd_sse.c -o bin/vecadd_sse
gcc -O3 -mavx2 -mfpmath=sse -march=x86-64 -mtune=native src/vecadd_avx.c -o bin/vecadd_avx
```

## 2) Build Gem5 (once)
```bash
# Example layout
cd ~
git clone https://gem5.googlesource.com/public/gem5
cd gem5
# (On Ubuntu/WSL make sure deps are installed: scons, python3-dev, etc.)
scons build/X86/gem5.opt -j$(nproc)
```

## 3) Run experiments
From your package folder in WSL:
```bash
cd /mnt/c/Users/dkuma/Downloads/DLP_Gem5_Simulation_Package

# Single run:
./scripts/run_gem5.sh ~/gem5 DerivO3CPU vecadd_avx 1048576 10

# Bulk (all CPUs × all variants):
./scripts/run_all.sh ~/gem5 1048576 10
```
Each run creates a folder like `results/DerivO3CPU_vecadd_avx/` with `stats.txt` and standard Gem5 outputs.

### Notes
* We use Gem5 SE mode with `configs/example/se.py`, L1/L2 caches, and 2GB memory.
* You can add `-I <max_insts>` to cap runtime if desired (see `scripts/run_gem5.sh` source).

## 4) Parse and plot results
```bash
# Convert all stats.txt under results/ into one CSV
python3 scripts/parse_stats.py results/ results/all_stats.csv

# Make charts (IPC and sim_seconds)
python3 scripts/plot.py results/all_stats.csv plots/
```
The PNGs will be written into `plots/`.

## 5) Reproducibility
All programs accept the same CLI:
```
./bin/vecadd_* -n <N> -r <repeats> -seed <seed>
```
* `-n` controls vector length (default 1,048,576 doubles).
* `-r` repeats the kernel to increase instruction counts (default 10).
* `-seed` initializes the random inputs to keep checksums consistent across variants.

## 6) Mapping to your Windows paths
Your Windows folder:
```
C:\Users\dkuma\Downloads\DLP_Gem5_Simulation_Package
```
is mounted in WSL at:
```
/mnt/c/Users/dkuma/Downloads/DLP_Gem5_Simulation_Package
```
Stick to the WSL path for building/running and the results will be visible in File Explorer on Windows.

## 7) Troubleshooting
* If `gem5.opt` path differs, pass the correct `<gem5_root>` to the scripts.
* If IPC/CPI keys differ between Gem5 versions, `parse_stats.py` will leave missing values blank. You can add/remove metric names in `METRICS`.
* If you see `illegal instruction` when running AVX2 natively (not in Gem5), compile without `-mavx2`. Under Gem5, the ISA support is modeled; use an ISA that matches the CPU model and binary.
