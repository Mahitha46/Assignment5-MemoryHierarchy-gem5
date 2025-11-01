#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./scripts/run_gem5.sh <gem5_root> <cpu_type> <variant> [N] [repeats]
# Example:
#   ./scripts/run_gem5.sh ~/gem5 O3CPU vecadd_avx 1048576 10
#
# It runs: build/X86/gem5.opt configs/deprecated/example/se.py -c ./bin/<variant> -o "-n N -r repeats"
# with a cache hierarchy and L2. Results go to results/<cpu>_<variant>/

if [[ $# -lt 3 ]]; then
  echo "Usage: $0 <gem5_root> <cpu_type> <variant> [N] [repeats]"
  exit 1
fi

GEM5_ROOT="$1"
CPU="$2"                # TimingSimpleCPU, MinorCPU, DerivO3CPU
VARIANT="$3"            # vecadd_scalar | vecadd_sse | vecadd_avx
N="${4:-1048576}"
REPEATS="${5:-10}"

SEPY="$GEM5_ROOT/configs/deprecated/example/se.py"
BIN="./bin/${VARIANT}"
OUTDIR="results/${CPU}_${VARIANT}"

mkdir -p "$OUTDIR"

# Note: -I lets you cap max instructions if desired (commented by default).
"$GEM5_ROOT/build/X86/gem5.opt" \
  --outdir="$OUTDIR" \
  "$SEPY" \
  -c "$BIN" \
  --options="-n ${N} -r ${REPEATS}" \
  --cpu-type="${CPU}" \
  --caches --l2cache \
  --l1i_size=32kB --l1d_size=32kB --l2_size=1MB \
  --mem-size=2GB

echo "Done. See ${OUTDIR}/stats.txt"
