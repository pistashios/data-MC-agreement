## Physics goal
Data–MC agreement on the BDT input variables and on the BDT score itself
(`bdt_nf3`) is a prerequisite for trusting the BDT-based WW selection that
feeds the 2PC measurement. A mismatch in the Data/MC ratio — particularly in
regions populated by signal — would imply that the MC mis-models the WW
selection, which biases the signal efficiency used to extract the 2PC. The
purpose of this task is to produce **Data/MC ratio plots as a first-pass
diagnostic** to spot any such mismodeling, broken down by ECM and by
track-multiplicity interval so we can localize where any disagreement comes
from.

## Scope
- Restricted to the `ntrkskim30 / nobdt` skim — see the top-level
  [`README.md`](../README.md#input-dataset-paths) for the file glob and
  path structure.
- Comparisons are done within a single ECM at a time.

## Variables to plot
- `d2`, `Mvis`, `missP`, `cW`, `Sphericity` (BDT input features)
- `bdt_nf3` (BDT score)

## Track-multiplicity bins
Four bins on `nChargedParticleHP`:
- inclusive
- `[30, 40)`
- `[40, 50)`
- `[50, 999)`

## ECM points
183, 189, 192, 205 GeV — each handled independently.

## How to run

```bash
make
./run.sh
```

This workflow is two compiled C++ programs plus a wire-up bash script;
see the layout below for the file structure.

### Layout
```
task_01_bdt_dataMC/
├── README.md
├── Makefile                # src/*.cc → bin/*.exe (links against ROOT)
├── src/
│   ├── analyze.cc          # skim → TH1Ds split by (variable, mult bin)
│   └── plot.cc             # histograms → multipanel Data/MC + ratio canvas
├── include/
│   └── variables.h         # variable + mult-bin lists for plot.cc to iterate
├── bin/                    # built executables
├── histograms/             # analyze output: hist_ecm<E>.root
├── plots/                  # plot output: ecm<E>_<var>.pdf
└── run.sh                  # resolve paths, then loop ECM → analyze → plot
```

### Per-step responsibilities

**`src/analyze.cc`**
- CLI: `analyze.exe --ecm <ECM> --data <data.root> --mc <mc.root> --out <out.root>`
- Opens the two input files, reads tree `t` from each, and fills one TH1D
  per `(variable, multBin, sample)` named `<var>__<multBin>__<sample>`.
- Writes every filled histogram to the output file.

**`src/plot.cc`**
- CLI: `plot.exe --ecm <ECM> --in <hist.root>`
- For each variable in `include/variables.h`, builds a multipanel canvas
  (one panel per mult bin, each with a Data/MC overlay + ratio sub-panel).
- Skips writing a PDF for variables with no usable panels in the input.
- Writes one PDF per variable: `plots/ecm<ECM>_<var>.pdf`.

**`include/variables.h`**
- The list `plot.cc` uses to know which histograms to look for. Note: the
  practice `analyze.cc` does **not** read this file — it hardcodes histogram
  names following the `<var>__<multBin>__<sample>` pattern. If you add or
  remove variables in `analyze.cc`, update this header too so `plot.cc`
  picks up the change.

**`run.sh`**
- Resolves the per-ECM data and MC paths, then for each ECM calls
  `analyze.exe` followed by `plot.exe`.

## Expected outputs
- `histograms/hist_ecm<E>.root` — one file per ECM containing TH1Ds keyed by
  `(variable, multBin, sample)`.
- `plots/ecm<E>_<var>.pdf` — one multipanel canvas per (ECM × variable),
  showing Data vs. MC overlay + Data/MC ratio panel, one panel per
  multiplicity bin.
