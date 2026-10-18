#!/usr/bin/env bash
# Build, then for each ECM resolve the input file paths and run
# analyze.exe -> plot.exe. ROOT environment must already be sourced
# (see top-level README).

set -euo pipefail

cd "$(dirname "$0")"

make

SKIM_BASE=/data2/janicechen/StudyMult_pr13_integration_clean/skimfiles/LEP2
FNAME=EnergyCut_ge_100_ntrkskim30_nobdt_skim_all.root

mkdir -p histograms plots

for ecm in 183 189 192 205; do
    case "$ecm" in
        183) year=1997 ;;
        189) year=1998 ;;
        192) year=1999 ;;
        205) year=2000 ;;
        *)   echo "unknown ECM: $ecm"; exit 1 ;;
    esac

    data_file="$SKIM_BASE/Data/$year/$ecm/$FNAME"
    mc_file="$SKIM_BASE/MC/$year/$ecm/$FNAME"
    out_file="histograms/hist_ecm${ecm}.root"

    [[ -f "$data_file" ]] || { echo "[run] missing data file: $data_file"; exit 1; }
    [[ -f "$mc_file"   ]] || { echo "[run] missing mc file:   $mc_file";   exit 1; }

    echo "=== ECM = ${ecm} GeV (year ${year}) ==="
    ./bin/analyze.exe --ecm "$ecm" --data "$data_file" --mc "$mc_file" --out "$out_file"
    ./bin/plot.exe    --ecm "$ecm" --in   "$out_file"
done

./bin/chi2.exe

echo "All ECM points done. See plots/ for output PDFs."
