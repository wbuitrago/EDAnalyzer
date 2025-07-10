#include "TSystem.h"
#include <iostream>
#include <vector>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>

void constituents_image() {
    TFile *file = TFile::Open("./jetConstituents/output_prova.root");

    TTree *tree = (TTree*)file->Get("JetAnalysisTree");
    TTreeReader reader(tree);
    
    TTreeReaderArray<float> pf_pt(reader, "pf_pt");
    TTreeReaderArray<float> pf_eta(reader, "pf_eta");
    TTreeReaderArray<float> pf_phi(reader, "pf_phi");
    TTreeReaderArray<int> pf_fatjetIdx(reader, "pf_fatjetIdx");

    int fatjet_count;

    int Nbins = 100;
    std::vector<float> pf_pt_sum(Nbins * Nbins, 0);  
    TH1F *h_N_constituents= new TH1F("h_N_constituents", "h_N_constituents", 100,0,100);
    TH2F* h_pf_scatter_eta_phi_multiplicity = new TH2F(
        "h_pf_scatter_eta_phi_multiplicity", 
        "PF Particles - eta vs phi multiplicity",
        Nbins, 0., 2.4,  
        Nbins, -3.14, 3.14 
    );

    Long64_t nEntries = reader.GetEntries();
    std::cout<<nEntries<<std::endl;
    int pf_entries=0;
    for (Long64_t entry = 0; entry < 50; ++entry) {
        reader.SetEntry(entry);
        if(pf_pt.GetSize()!=0) h_N_constituents->Fill(pf_pt.GetSize());
        for (size_t i = 0; i < pf_pt.GetSize(); ++i) {
            float pt = pf_pt[i];
            float eta = pf_eta[i];
            float phi = pf_phi[i];

            float eta_bin = 2.4 / Nbins;
            float phi_bin = 6.28 / Nbins;
            int eta_bin_idx = int((abs(eta)/ eta_bin));
            int phi_bin_idx = int((phi + 3.14) / phi_bin);

            if (eta_bin_idx >= 0 && eta_bin_idx < Nbins && phi_bin_idx >= 0 && phi_bin_idx < Nbins) {
                int bin_index = eta_bin_idx * Nbins + phi_bin_idx;
                pf_pt_sum[bin_index] += pt;  
                h_pf_scatter_eta_phi_multiplicity->Fill(eta_bin_idx, phi_bin_idx );
                pf_entries++;
            }
        }
    }
    TH2F* h_pf_scatter_eta_phi_pt = new TH2F(
        "h_pf_scatter_eta_phi_pt", 
        "Constituents - eta vs phi with pt sum",
        Nbins, 0., 4.5,  
        Nbins, -3.14, 3.14 
    );

    for (int phi_bin = 0; phi_bin < Nbins; phi_bin++) {
        for (int eta_bin = 0; eta_bin < Nbins; eta_bin++) {
            int bin_index = eta_bin * Nbins + phi_bin;
            h_pf_scatter_eta_phi_pt->SetBinContent(phi_bin + 1, eta_bin + 1, pf_pt_sum[bin_index]);
        }
    }
    TCanvas* c = new TCanvas();
    c->cd();
    //Double_t levels[] = {30, 80, 120,150,200,250,300,400,500,800,1300,1800,2000, 2500,3500,4500,7000};  // Adjust levels as needed
    //h_pf_scatter_eta_phi_pt->SetContour(16, levels); // Set the number of contours to 5 and set the levels
    h_pf_scatter_eta_phi_pt->Draw("COLZ4");
    h_pf_scatter_eta_phi_pt->GetXaxis()->SetTitle("#eta");
    h_pf_scatter_eta_phi_pt->GetYaxis()->SetTitle("#phi");
    h_pf_scatter_eta_phi_pt->GetZaxis()->SetTitle("p_{T}");
    h_pf_scatter_eta_phi_pt->GetZaxis()->SetTitleOffset(0.4);
    h_pf_scatter_eta_phi_pt->SetStats(0);
    c->SetLogz();

    c->SaveAs("./images_constituents/pf_image_eta_phi_pt.pdf");

    TCanvas *c2 = new TCanvas();
    c2->cd();
    h_N_constituents->Draw();
    c2->SaveAs("./images_constituents/fatjet_multiplicity.pdf");

    TCanvas *c3 = new TCanvas();
    c3->cd();
    h_pf_scatter_eta_phi_multiplicity->Draw();
    c3->SaveAs("./images_constituents/pf_image_eta_phi_multiplicity.pdf");

    TFile *f= new TFile ("out.root", "recreate");
    f->cd();
    h_pf_scatter_eta_phi_pt->Write();
    f->Close();
    file->Close();
    std::cout<<"pf_entries"<<pf_entries<<std::endl;
    gROOT->ProcessLine(".q");
}
