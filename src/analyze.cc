// analyze.cc — read data and MC skim files for one ECM, fill TH1Ds split
// by (variable, multiplicity bin, sample), and write them to one .root
// file.
//
// CLI:
//   analyze.exe --ecm <ECM> --data <data.root> --mc <mc.root> --out <out.root>
//
// All filesystem paths are passed in from run.sh — this program contains
// no hardcoded paths.
//
// Histogram naming convention used throughout: "<var>__<multbin>__<sample>"
//   var      : a branch name in the skim, e.g. "d2", "Mvis", "bdt_nf3"
//   multbin  : "inclusive" (every event) or "30to40" / "40to50" / "50to999"
//   sample   : "data" or "mc"
// plot.cc reads histograms back by these exact names. If you change the
// pattern here, also update include/variables.h and plot.cc.
//
// Branch types in the skim (from `rootls -t <file>`):
//   nChargedParticleHP : Int_t   (32-bit signed integer; HP = High Purity)
//   passesISR          : Bool_t  (event passes the ISR-clean cut)
//   d2, Mvis, missP, cW, Sphericity, bdt_nf3 : Float_t  (32-bit float)

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <iomanip>

#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TTree.h>

#include "variables.h"

std::string histKey(const std::string& var,
                    const std::string& multBin,
                    const std::string& sample) {
    return var + "__" + multBin + "__" + sample;
}

std::string histKeyVar(const std::string& var,
                    const std::string& multBin,
                    const std::string& sample) {
    return "h_" + var + "_" + multBin + "_" + sample;
}

int main(int argc, char** argv) {
    // =====================================================================
    // 1. Parse CLI args
    // =====================================================================
    std::string dataPath, mcPath, outPath;
    int ecm = 0;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if      (a == "--ecm"  && i + 1 < argc) ecm      = std::stoi(argv[++i]);
        else if (a == "--data" && i + 1 < argc) dataPath = argv[++i];
        else if (a == "--mc"   && i + 1 < argc) mcPath   = argv[++i];
        else if (a == "--out"  && i + 1 < argc) outPath  = argv[++i];
    }
    if (ecm == 0 || dataPath.empty() || mcPath.empty() || outPath.empty()) {
        std::cerr << "Usage: analyze.exe --ecm <ECM> --data <data.root> "
                     "--mc <mc.root> --out <out.root>\n";
        return 1;
    }
    std::cout << "[args] ecm=" << ecm
              << "  data="     << dataPath
              << "  mc="       << mcPath
              << "  out="      << outPath << "\n";

    // =====================================================================
    // 2. Open input files and grab their TTrees
    // =====================================================================
    TFile fData(dataPath.c_str()); // grabs the TFile for the data
    TFile fMC  (mcPath.c_str()); // for the monte carlo
    TTree* tData = static_cast<TTree*>(fData.Get("t")); // the tree is called "t", so that's how it's grabbed
    TTree* tMC   = static_cast<TTree*>(fMC  .Get("t"));
    if (!tData || !tMC) {
        std::cerr << "[error] could not find tree 't' in one of the inputs\n";
        return 1;
    }

    // =====================================================================
    // 3. Open output file and create histograms — YOUR JOB
    // =====================================================================
    // Open the output ROOT file FIRST so the TH1Ds you `new` below auto-
    // attach to it. When fout is closed at the end, ROOT writes them and
    // cleans them up — that's why we don't need a matching `delete` for
    // each `new`.
    TFile fout(outPath.c_str(), "RECREATE");

    // creating a TTree to output the chi-squared
    TTree* chi2Tree = new TTree("chi2", "chiSquared summary");
    int tree_ecm;
    char tree_var[64];
    char tree_multBin[64];

    double tree_chi2;
    double tree_chi2_r;
    int tree_ndf;
    chi2Tree->Branch("ecm", &tree_ecm, "ecm/I");

    chi2Tree->Branch("variable", tree_var, "variable/C");
    chi2Tree->Branch("multBin", tree_multBin, "multBin/C");

    chi2Tree->Branch("chi2", &tree_chi2, "chi2/D");
    chi2Tree->Branch("ndf", &tree_ndf, "ndf/I");
    chi2Tree->Branch("chi2_r", &tree_chi2_r, "chi2_r/D");

    // Enable Sumw2 by default so every histogram tracks proper Poisson
    // errors (needed for correct Data/MC ratio uncertainties).
    TH1::SetDefaultSumw2();

    // Example: the two TH1Ds for the d2 inclusive panel (one for data, one
    // for MC). Use these two lines as the pattern for the other 46 you need
    // to declare.
    // TH1D* h_d2_inclusive_data = new TH1D("d2__inclusive__data", ";d2;Events", 50, 0.0, 1.0);
    // TH1D* h_d2_inclusive_mc   = new TH1D("d2__inclusive__mc",   ";d2;Events", 50, 0.0, 1.0);

    // YOUR TASK: declare the remaining TH1D pointers — one per (variable,
    // mult bin, sample). With 6 variables × 4 mult bins × 2 samples, that
    // is 48 TH1Ds total; the two above leave 46 to go.
    //
    // Naming convention: name the C++ pointer h_<var>_<multbin>_<sample>
    // and use the same triple, joined by "__", inside the TH1D constructor.
    //
    // Suggested binning ranges:
    //   d2, cW, Sphericity, bdt_nf3 :  50 bins,  [0.0, 1.0]
    //   Mvis                        :  60 bins,  [50.0, 220.0]
    //   missP                       :  50 bins,  [0.0, 80.0]

    // declare the other 10 inclusive TH1Ds
    // (Mvis, missP, cW, Sphericity, bdt_nf3 × {data, mc}).

    // TH1D* h_Mvis_inclusive_data = new TH1D("Mvis__inclusive__data", ";Mvis;Events", 60, 50.0, 220.0);
    // TH1D* h_Mvis_inclusive_mc   = new TH1D("Mvis__inclusive__mc", ";Mvis;Events", 60, 50.0, 220.0);


    // create a map with the key as the original pointer value, and the TH1D as the key

    std::map<std::string, TH1D*> TH1DMap;

    for (const auto& var : kVariables()) {
        for (const auto& multBin : kMultBins()) {
            for (const std::string sample : {"mc", "data"}) {
                std::string keyName   = histKeyVar(var.name, multBin.name, sample);
                std::string valueName = histKey(var.name, multBin.name, sample);

                TH1DMap[keyName] = new TH1D(valueName.c_str(), (";" + var.name + ";Events").c_str(), var.nbins, var.xmin, var.xmax);
            }
        }
    }

    // [TODO: exercise 3] declare the 36 differential-mult-bin TH1Ds
    //                    (6 variables × {30to40, 40to50, 50to999} × {data, mc}).

    // =====================================================================
    // 4. Event loops — YOUR JOB
    // =====================================================================
    //
    // For each event that passes the ISR cut, fill the inclusive
    // histograms unconditionally and the differential-mult-bin histograms
    // only when nChargedParticleHP falls in the matching range. Mirror
    // the same pattern in the MC loop using the *_mc histograms.

    // The same local variables are shared between the data loop (4a) and
    // the MC loop (4b) — declared here once at function scope.
    Int_t   nCh        = 0;
    Bool_t  passesISR  = false;
    Float_t d2         = 0;
    Float_t Mvis       = 0;
    Float_t missP      = 0;
    Float_t cW         = 0;
    Float_t Sphericity = 0;
    Float_t bdt_nf3    = 0;
    // [TODO: exercise 2] declare Float_t variables for Mvis, missP, cW, Sphericity, bdt_nf3.

    // ---- (4a) Data loop --------------------------------------------------
    // Wire up the branches on tData.
    tData->SetBranchAddress("nChargedParticleHP", &nCh);
    tData->SetBranchAddress("passesISR",          &passesISR);
    tData->SetBranchAddress("d2",                 &d2);
    // [TODO: exercise 2] SetBranchAddress on tData for the other five variables — same pattern.
    tData->SetBranchAddress("Mvis",               &Mvis);
    tData->SetBranchAddress("missP",              &missP);
    tData->SetBranchAddress("cW",                 &cW);
    tData->SetBranchAddress("Sphericity",         &Sphericity);
    tData->SetBranchAddress("bdt_nf3",            &bdt_nf3);


    std::cout << "[loop] data entries=" << tData->GetEntries() << "\n";
    for (Long64_t i = 0; i < tData->GetEntries(); ++i) {
        tData->GetEntry(i);                  // load branch values for event i
        if (!passesISR) continue;            // skip events failing the ISR cut


        std::map<std::string, double> values = {
            {"d2", d2},
            {"Mvis", Mvis},
            {"missP", missP},
            {"cW", cW},
            {"Sphericity", Sphericity},
            {"bdt_nf3", bdt_nf3}
        };

        for (const auto& var : kVariables()) {
            for (const auto& multBin : kMultBins()) {
                if (multBin.high == -1 || (multBin.low <= nCh && multBin.high >= nCh)) {
                    TH1DMap[histKeyVar(var.name, multBin.name, "data")]->Fill(values.at(var.name));
                }
            }
        }
    }

    // ---- MC loop ----------------------------------------------------
    tMC->SetBranchAddress("nChargedParticleHP", &nCh);
    tMC->SetBranchAddress("passesISR",          &passesISR);
    tMC->SetBranchAddress("d2",                 &d2);
    tMC->SetBranchAddress("Mvis",               &Mvis);
    tMC->SetBranchAddress("missP",              &missP);
    tMC->SetBranchAddress("cW",                 &cW);
    tMC->SetBranchAddress("Sphericity",         &Sphericity);
    tMC->SetBranchAddress("bdt_nf3",            &bdt_nf3);

    std::cout << "[loop] mc entries=" << tMC->GetEntries() << "\n";
    for (Long64_t i = 0; i < tMC->GetEntries(); ++i) {
        tMC->GetEntry(i);                    // load branch values for event i
        if (!passesISR) continue;            // skip events failing the ISR cut

        std::map<std::string, double> values = {
            {"d2", d2},
            {"Mvis", Mvis},
            {"missP", missP},
            {"cW", cW},
            {"Sphericity", Sphericity},
            {"bdt_nf3", bdt_nf3}
        };

        for (const auto& var : kVariables()) {
            for (const auto& multBin : kMultBins()) {
                if (multBin.high == -1 || (multBin.low <= nCh && multBin.high >= nCh)) {
                    TH1DMap[histKeyVar(var.name, multBin.name, "mc")]->Fill(values.at(var.name));
                }
            }
        }
    }


    // =====================================================================
    // 5. Write output
    // =====================================================================
    fout.Write();
    fout.Close();
    std::cout << "[done] wrote " << outPath << "\n";
    return 0;
}
