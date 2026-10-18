// plot.cc — read an `analyze` output file and produce, for each variable,
// a multipanel Data/MC overlay + ratio canvas.
//
// CLI:    plot.exe --ecm <ECM> --in <hist.root>
// Output: plots/ecm<ECM>_<var>.pdf, one per variable.
//
// Layout per PDF: a 2-row × N-column canvas, where N is the number of
// multiplicity bins from include/variables.h. Top row = Data (markers)
// overlaid with MC (filled histogram, normalized to data integral). Bottom
// row = Data/MC ratio with statistical error bars, dashed reference line
// at 1.
//
// If a variable has no usable panels in the input file (e.g. the analyze
// step hasn't filled them yet), the PDF for that variable is skipped
// entirely rather than producing an empty canvas.

#include <iostream>
#include <memory>
#include <string>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLatex.h>

#include "variables.h"

namespace {

std::string histKey(const std::string& var,
                    const std::string& multBin,
                    const std::string& sample) {
    return var + "__" + multBin + "__" + sample;
}

void stylePad() {
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.04);
    gPad->SetTopMargin(0.1);
    gPad->SetBottomMargin(0.14);
}

void drawOverlay(TH1D* hData, TH1D* hMC, const std::string& title) {
    stylePad();
    if (hMC->Integral() > 0)
        hMC->Scale(hData->Integral() / hMC->Integral());

    const double yMax = 1.4 * std::max(hData->GetMaximum(), hMC->GetMaximum());

    hMC->SetFillColorAlpha(kAzure - 4, 0.5);
    hMC->SetLineColor(kAzure + 2);
    hMC->SetLineWidth(2);
    hMC->SetStats(0);
    hMC->SetTitle(title.c_str());
    hMC->SetTitle("");
    hMC->GetYaxis()->SetTitle("Events (MC scaled to data)");
    hMC->GetYaxis()->SetTitleOffset(1.5);
    hMC->SetMaximum(yMax);
    hMC->Draw("hist");

    hData->SetMarkerStyle(20);
    hData->SetMarkerSize(0.8);
    hData->SetLineColor(kBlack);
    hData->SetStats(0);
    hData->Draw("EX0 same");

    hMC->GetYaxis()->SetTitleSize(0.07);
    hMC->GetYaxis()->SetLabelSize(0.07);
    hData->GetYaxis()->SetTitleSize(0.07);
    hData->GetYaxis()->SetLabelSize(0.07);
    hMC->GetXaxis()->SetTitleSize(0.07);
    hMC->GetXaxis()->SetLabelSize(0.07);
    hData->GetXaxis()->SetTitleSize(0.07);
    hData->GetXaxis()->SetLabelSize(0.07);

    auto* leg = new TLegend(0.70, 0.71, 0.95, 0.85);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(hData, "Data", "pe");
    leg->AddEntry(hMC,   "MC",   "f");
    leg->SetTextSize(0.07);
    leg->Draw();

    // after drawing histogram and legend
    TLatex latex;
    latex.SetNDC();
    latex.SetTextAlign(22);    // centered
    latex.SetTextSize(0.08);   // adjust as needed
    latex.DrawLatex(0.5, 0.95, title.c_str());
}

void drawRatio(TH1D* hData, TH1D* hMC) {
    stylePad();
    auto* hRatio = static_cast<TH1D*>(hData->Clone());
    hRatio->SetDirectory(nullptr);
    hRatio->Divide(hMC);
    hRatio->SetStats(0);
    hRatio->SetTitle("");
    hRatio->GetYaxis()->SetTitle("Data / MC");
    hRatio->GetYaxis()->SetTitleSize(0.07);
    hRatio->GetYaxis()->SetTitleOffset(1.1);
    hRatio->GetYaxis()->SetLabelSize(0.07);
    //hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);
    hRatio->GetYaxis()->SetRangeUser(0.0, 5.0);
    hRatio->GetYaxis()->SetNdivisions(505);
    hRatio->GetXaxis()->SetTitleSize(0.07);
    hRatio->GetXaxis()->SetLabelSize(0.07);
    hRatio->SetMarkerStyle(20);
    hRatio->SetMarkerSize(0.8);
    hRatio->SetLineColor(kBlack);
    hRatio->Draw("EX0");

    auto* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1.0,
                           hRatio->GetXaxis()->GetXmax(), 1.0);
    line->SetLineStyle(2);
    line->SetLineColor(kRed + 1);
    line->Draw();
}

}  // namespace

int main(int argc, char** argv) {
    int ecm = 0;
    std::string inPath;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if      (a == "--ecm" && i + 1 < argc) ecm    = std::stoi(argv[++i]);
        else if (a == "--in"  && i + 1 < argc) inPath = argv[++i];
    }
    if (ecm == 0 || inPath.empty()) {
        std::cerr << "Usage: plot.exe --ecm <ECM> --in <hist.root>\n";
        return 1;
    }

    TFile fin(inPath.c_str());
    if (fin.IsZombie()) {
        std::cerr << "[plot] cannot open " << inPath << "\n";
        return 1;
    } // this just makes sure we can open the file in the path given

    //gStyle->SetOptStat(0);
    //gStyle->SetTitleFontSize(0.1);
    gSystem->mkdir("plots", /*recursive=*/true);

    const int ncol = static_cast<int>(kMultBins().size());
    for (const auto& var : kVariables()) {
        TCanvas c(("c_" + var.name).c_str(),
                  ("c_" + var.name).c_str(),
                  400 * ncol, 600);
        c.Divide(ncol, 2, 0.001, 0.001);

        int nRendered = 0;
        for (int col = 0; col < ncol; ++col) {
            const auto& mb = kMultBins()[col];
            auto* hData = static_cast<TH1D*>(
                fin.Get(histKey(var.name, mb.name, "data").c_str()));
            auto* hMC = static_cast<TH1D*>(
                fin.Get(histKey(var.name, mb.name, "mc").c_str()));
            if (!hData || !hMC
             || hData->GetEntries() == 0 || hMC->GetEntries() == 0) {
                std::cerr << "[plot] missing or empty histogram for "
                          << var.name << " / " << mb.name << "\n";
                continue;
            }

            const std::string panelTitle =
                var.name + " (#sqrt{s}=" + std::to_string(ecm)
                + " GeV, mult " + mb.name + ")";

            c.cd(col + 1);             // top row: overlay
            drawOverlay(hData, hMC, panelTitle);
            c.cd(col + 1 + ncol);      // bottom row: ratio
            drawRatio(hData, hMC);
            ++nRendered;
        }

        if (nRendered == 0) {
            std::cout << "[plot] skipping " << var.name
                      << " (no usable panels in input)\n";
            continue;
        }

        const TString outPath = TString::Format(
            "plots/ecm%d_%s.pdf", ecm, var.name.c_str());
        c.SaveAs(outPath);
        std::cout << "[plot] wrote " << outPath << "\n";
    }

    return 0;
}
