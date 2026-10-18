#ifndef UROPLEP2WW_TASK01_VARIABLES_H
#define UROPLEP2WW_TASK01_VARIABLES_H

#include <string>
#include <vector>

// One place to declare every plotted variable and every multiplicity bin.
// Edit ranges / bin counts here; analyze.cc and plot.cc pick them up
// automatically.

struct VarDef {
    std::string name;   // branch name in the skim TTree (case-sensitive)
    int         nbins;
    double      xmin;
    double      xmax;
};

inline const std::vector<VarDef>& kVariables() {
    static const std::vector<VarDef> x = {
        // name           nbins  xmin   xmax    (ranges are first-pass guesses; tune as needed)
        {"d2",            8,    0.0,   1.0},
        {"Mvis",          10,    50.0,  220.0},
        {"missP",         8,    0.0,   80.0},
        {"cW",            8,    0.0,   1.0},
        {"Sphericity",    8,    0.0,   1.0},
        {"bdt_nf3",       8,    0.0,   1.0},
    };
    static const std::vector<VarDef> v = {
        // name           nbins  xmin   xmax    (ranges are first-pass guesses; tune as needed)
        {"d2",            12,    0.0,   1.0},
        {"Mvis",          15,    50.0,  220.0},
        {"missP",         12,    0.0,   80.0},
        {"cW",            12,    0.0,   1.0},
        {"Sphericity",    12,    0.0,   1.0},
        {"bdt_nf3",       12,    0.0,   1.0},
    };
    static const std::vector<VarDef> w = {
        // name           nbins  xmin   xmax    (ranges are first-pass guesses; tune as needed)
        {"d2",            18,    0.0,   1.0},
        {"Mvis",          20,    50.0,  220.0},
        {"missP",         18,    0.0,   80.0},
        {"cW",            18,    0.0,   1.0},
        {"Sphericity",    18,    0.0,   1.0},
        {"bdt_nf3",       18,    0.0,   1.0},
    };
    static const std::vector<VarDef> k = {
        // name           nbins  xmin   xmax    (ranges are first-pass guesses; tune as needed)
        {"d2",            25,    0.0,   1.0},
        {"Mvis",          30,    50.0,  220.0},
        {"missP",         25,    0.0,   80.0},
        {"cW",            25,    0.0,   1.0},
        {"Sphericity",    25,    0.0,   1.0},
        {"bdt_nf3",       25,    0.0,   1.0},
    };
    return k;
}

struct MultBin {
    std::string name;   // used in histogram keys and filenames
    int         low;    // inclusive lower edge on nChargedParticle
    int         high;   // exclusive upper edge; use -1 to mean "no upper cut"
};

inline const std::vector<MultBin>& kMultBins() {
    static const std::vector<MultBin> k = {
        // The "inclusive" bin contains every event in the skim (which already
        // has the implicit nChargedParticle >= 30 cut from `ntrkskim30`).
        {"inclusive",  30,  -1},
        {"30to40",     30,  40},
        {"40to50",     40,  50},
        {"50to999",    50,  999},
    };
    static const std::vector<MultBin> v = {
        // The "inclusive" bin contains every event in the skim (which already
        // has the implicit nChargedParticle >= 30 cut from `ntrkskim30`).
        {"inclusive",  30,  -1},
        {"30to35",     30,  35},
        {"35to40",     35,  40},
        {"40to45",     40,  45},
        {"45to50",     45,  50},
        {"50to55",     50,  55},
        {"55to999",    55, 999},
    };
    return k;
}

#endif  // UROPLEP2WW_TASK01_VARIABLES_H
