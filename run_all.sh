#!/usr/bin/env bash
set -euo pipefail
# Bulk runner over CPU types and variants

GEM5_ROOT="${1:-$HOME/gem5}"
N="${2:-1048576}"
REPEATS="${3:-10}"

declare -a CPUS=("TimingSimpleCPU" "MinorCPU" "DerivO3CPU")
declare -a VARS=("vecadd_scalar" "vecadd_sse" "vecadd_avx")

for c in "${CPUS[@]}"; do
  for v in "${VARS[@]}"; do
    echo "=== Running $c / $v ==="
    ./scripts/run_gem5.sh "$GEM5_ROOT" "$c" "$v" "$N" "$REPEATS"
  done
done
