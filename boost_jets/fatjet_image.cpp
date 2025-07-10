#include "TSystem.h"
#include <iostream>
#include <vector>
#include <TH2F.h>
#include <TCanvas.h>
#include <ROOT/RDF/RInterface.hxx>
#include <ROOT/RDataFrame.hxx>


// root files after FatJet_Sel.py module //
const TString rdf_ssWW = "WW_cleanFatJet.root";
const TString rdf_ZZ = "ZZ_cleanFatJet.root";



// function to build the scatter plot, computes the sum of pt for each eta-phi bin for all the events
std::vector<float> PT_Sum_FatJet(ROOT::RDataFrame& df, int Nbins, TString phi_column, TString eta_column, TString pt_column) {
    std::vector<float> FatJet_pt_sum(Nbins * Nbins, 0);  // 2D histogram stored as a 1D vector
    
    df.Foreach([&FatJet_pt_sum, Nbins, eta_column](const ROOT::RVec<float>& phi_fatjets, 
                                       const ROOT::RVec<float>& eta_fatjets, 
                                       const ROOT::RVec<float>& pt_fatjets) {
      if (pt_fatjets.size() > 0) {
        float eta_bin;
        float phi_bin;
        if(eta_column.Contains("central")){
             eta_bin = 2.4/ Nbins; // Eta range is -2.4 to 2.4
             phi_bin = 6.28/Nbins;
        }
        else{ 
             eta_bin = (4.5 - 2.4 ) / Nbins;
             phi_bin = 6.28/Nbins;
        }
        for (size_t k = 0; k < pt_fatjets.size(); k++) {
            float phi = phi_fatjets[k];
            float eta = (eta_fatjets[k]);
            float pt = pt_fatjets[k];
            int eta_bin_idx;
            if(eta_column.Contains("central"))  eta_bin_idx = int(TMath::Abs(eta)/ eta_bin); 
            else   eta_bin_idx = int((TMath::Abs(eta) -2.4)/ eta_bin);
            int phi_bin_idx =int((phi + 3.14) / phi_bin); 
            if(eta_column.Contains("frw")) {std::cout<<eta_bin_idx<<std::endl;}

            if (eta_bin_idx >= 0 && eta_bin_idx < Nbins && phi_bin_idx >= 0 && phi_bin_idx < Nbins) {
                int bin_index = eta_bin_idx * Nbins + phi_bin_idx;
                FatJet_pt_sum[bin_index] += pt;
            }
        }
      }
    }, {phi_column.Data(), eta_column.Data(), pt_column.Data()});

    return FatJet_pt_sum;
}



void fatjet_image() {
    // square matrix
    int Nbins = 30;
    std::vector<float> ptSum_ssWW_central;
    std::vector<float> ptSum_ssWW_frw;
    std::vector<float> ptSum_ZZ_central;
    std::vector<float> ptSum_ZZ_frw;
    std::system("mkdir -p images");
    if (gSystem->AccessPathName("WW_cleanFatJet.root")) std::system("python3 FatJet_Sel.py ssWW");
    if (gSystem->AccessPathName("ZZ_cleanFatJet.root")) std::system("python3 FatJet_Sel.py ZZ");

    ROOT::RDataFrame df_ssWW("Events", rdf_ssWW.Data());
    ROOT::RDataFrame df_ZZ("Events", rdf_ZZ.Data());
    
    ptSum_ssWW_central = PT_Sum_FatJet(df_ssWW, Nbins, "FatJet_central_phi", "FatJet_central_eta", "FatJet_central_pt");
    ptSum_ssWW_frw = PT_Sum_FatJet(df_ssWW, Nbins, "FatJet_frw_phi", "FatJet_frw_eta", "FatJet_frw_pt");

    ptSum_ZZ_central = PT_Sum_FatJet(df_ZZ, Nbins, "FatJet_central_phi", "FatJet_central_eta", "FatJet_central_pt");
    ptSum_ZZ_frw = PT_Sum_FatJet(df_ZZ, Nbins, "FatJet_frw_phi", "FatJet_frw_eta", "FatJet_frw_pt");

    // Create histograms
    TH2F* h_central_scatter_ssWW_eta_phi_pt = new TH2F(
        "h_central_scatter_ssWW_eta_phi_pt", 
        "h_central_scatter_ssWW_eta_phi_pt", 
        Nbins, 0, 2.4,   
        Nbins, -3.14, 3.14  
    );
    TH2F* h_central_scatter_ZZ_eta_phi_pt = new TH2F(
        "h_central_scatter_ZZ_eta_phi_pt", 
        "h_central_scatter_ZZ_eta_phi_pt", 
        Nbins, 0, 2.4,   
        Nbins, -3.14, 3.14  
    );
    TH2F* h_frw_scatter_ssWW_eta_phi_pt = new TH2F(
        "h_frw_scatter_ssWW_eta_phi_pt", 
        "h_frw_scatter_ssWW_eta_phi_pt", 
        Nbins, 2.4, 4.5,   
        Nbins, -3.14, 3.14 
    );
    TH2F* h_frw_scatter_ZZ_eta_phi_pt = new TH2F(
        "h_frw_scatter_ZZ_eta_phi_pt", 
        "h_frw_scatter_ZZ_eta_phi_pt", 
        Nbins, 2.4, 4.5,   
        Nbins, -3.14, 3.14  
    );

    // Fill the histograms with the accumulated pt sums
    for (int phibin = 0; phibin < Nbins; phibin++) {
        for (int etabin = 0; etabin < Nbins; etabin++) {
            int bin_index = etabin * Nbins + phibin;
            h_central_scatter_ssWW_eta_phi_pt->SetBinContent(phibin + 1, etabin + 1, ptSum_ssWW_central[bin_index]);
            h_central_scatter_ZZ_eta_phi_pt->SetBinContent(phibin + 1, etabin + 1, ptSum_ZZ_central[bin_index]); 
            h_frw_scatter_ssWW_eta_phi_pt->SetBinContent(phibin + 1, etabin + 1, ptSum_ssWW_frw[bin_index]);
            h_frw_scatter_ZZ_eta_phi_pt->SetBinContent(phibin + 1, etabin + 1, ptSum_ZZ_frw[bin_index]); 
        }
    }

    TCanvas* c = new TCanvas();
    c->cd();
    h_central_scatter_ssWW_eta_phi_pt->Draw("COLZ");
    h_central_scatter_ssWW_eta_phi_pt->GetXaxis()->SetTitle("#eta");
    h_central_scatter_ssWW_eta_phi_pt->GetYaxis()->SetTitle("#phi");
    h_central_scatter_ssWW_eta_phi_pt->GetZaxis()->SetTitle("p^{sum}_{T}");
    h_central_scatter_ssWW_eta_phi_pt->SetStats(0);
    c->SaveAs("./images/fatjet_image_ssWW_central.pdf");

    TCanvas* c2 = new TCanvas();
    c2->cd();
    h_frw_scatter_ssWW_eta_phi_pt->Draw("COLZ");
    h_frw_scatter_ssWW_eta_phi_pt->GetXaxis()->SetTitle("#eta");
    h_frw_scatter_ssWW_eta_phi_pt->GetYaxis()->SetTitle("#phi");
    h_frw_scatter_ssWW_eta_phi_pt->GetZaxis()->SetTitle("p^{sum}_{T}");
    h_frw_scatter_ssWW_eta_phi_pt->SetStats(0);
    c2->SaveAs("./images/fatjet_image_ssWW_frw.pdf");

    TCanvas* c3 = new TCanvas();
    c3->cd();
    h_central_scatter_ZZ_eta_phi_pt->Draw("COLZ");
    h_central_scatter_ZZ_eta_phi_pt->GetXaxis()->SetTitle("#eta");
    h_central_scatter_ZZ_eta_phi_pt->GetYaxis()->SetTitle("#phi");
    h_central_scatter_ZZ_eta_phi_pt->GetZaxis()->SetTitle("p^{sum}_{T}");
    h_central_scatter_ZZ_eta_phi_pt->SetStats(0);
    c3->SaveAs("./images/fatjet_image_ZZ_central.pdf");

    TCanvas* c4 = new TCanvas();
    c4->cd();
    h_frw_scatter_ZZ_eta_phi_pt->Draw("COLZ");
    h_frw_scatter_ZZ_eta_phi_pt->GetXaxis()->SetTitle("#eta");
    h_frw_scatter_ZZ_eta_phi_pt->GetYaxis()->SetTitle("#phi");
    h_frw_scatter_ZZ_eta_phi_pt->GetZaxis()->SetTitle("p^{sum}_{T}");
    h_frw_scatter_ZZ_eta_phi_pt->SetStats(0);
    c4->SaveAs("./images/fatjet_image_ZZ_frw.pdf");

    gROOT->ProcessLine(".q");
}
