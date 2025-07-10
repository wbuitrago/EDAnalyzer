#include <memory>
#include <vector>
#include <string> // Added for std::string

// ROOT includes
#include "TMath.h"
#include "TString.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

// FWCore includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h" // For edm::LogError

// Data formats
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h" // For reco::PFCandidate
#include "DataFormats/Candidate/interface/Candidate.h" // For edm::Ptr<reco::Candidate>

class FatJetAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
    explicit FatJetAnalyzer(const edm::ParameterSet&);
    ~FatJetAnalyzer() override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override;

    void initializeHistograms();

    // Configuration parameters
    const bool saveHistograms_;
    const bool saveTree_;
    const double minFatJetPt_;
    const edm::InputTag fatjetsInputTag_; // Store the InputTag

    // Tokens
    const edm::EDGetTokenT<reco::PFJetCollection> fatjetsToken_;

    // Tree
    TTree* eventTree_;

    // --- Branches for the new TTree structure ---
    std::vector<float> fatjet_pt_;
    std::vector<float> fatjet_eta_;
    std::vector<float> fatjet_phi_;
    std::vector<float> fatjet_mass_;
    std::vector<int>   fatjet_Idx_;

    std::vector<float> pf_pt_;
    std::vector<float> pf_eta_;
    std::vector<float> pf_phi_;
    std::vector<int>   pf_IdxFatJet_;

    // Histograms (optional)
    TH1F* hFatJetPt_;
    TH1F* hFatJetEta_;
    TH1F* hFatJetPhi_;
    TH1F* hFatJetMass_;
    TH1F* hFatJetNConstituents_;
    TH2F* hFatJetEtaVsPhi_;
    TH2F* hFatJetPtVsEta_;
    TH2F* hFatJetConstituentPtVsEta_;
};

FatJetAnalyzer::FatJetAnalyzer(const edm::ParameterSet& iConfig) :
    saveHistograms_(iConfig.getParameter<bool>("saveHistograms")),
    saveTree_(iConfig.getParameter<bool>("saveTree")),
    minFatJetPt_(iConfig.getParameter<double>("minFatJetPt")),
    fatjetsInputTag_(iConfig.getParameter<edm::InputTag>("fatjets")), // Initialize here
    fatjetsToken_(consumes<reco::PFJetCollection>(fatjetsInputTag_))   // Use the stored InputTag
{
    usesResource("TFileService");
}

FatJetAnalyzer::~FatJetAnalyzer() {}

void FatJetAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<bool>("saveHistograms", true);
    desc.add<bool>("saveTree", true);
    desc.add<double>("minFatJetPt", 150.0);
    desc.add<edm::InputTag>("fatjets", edm::InputTag("ak8PFJetsPuppi"));
    descriptions.add("fatJetAnalyzer", desc);
}

void FatJetAnalyzer::beginJob() {
    edm::Service<TFileService> fs;

    if (saveTree_) {
        eventTree_ = fs->make<TTree>("FatJetAnalyzer_AOD", "FatJet and PF Candidate information");

        eventTree_->Branch("fatjet_pt", &fatjet_pt_);
        eventTree_->Branch("fatjet_eta", &fatjet_eta_);
        eventTree_->Branch("fatjet_phi", &fatjet_phi_);
        eventTree_->Branch("fatjet_mass", &fatjet_mass_);
        eventTree_->Branch("fatjet_Idx", &fatjet_Idx_);

        eventTree_->Branch("pf_pt", &pf_pt_);
        eventTree_->Branch("pf_eta", &pf_eta_);
        eventTree_->Branch("pf_phi", &pf_phi_);
        eventTree_->Branch("pf_IdxFatJet", &pf_IdxFatJet_);
    }

    if (saveHistograms_) {
        initializeHistograms();
    }
}

void FatJetAnalyzer::initializeHistograms() {
    edm::Service<TFileService> fs;

    hFatJetPt_ = fs->make<TH1F>("hFatJetPt", "FatJet p_{T} (p_{T} > minPt);p_{T} [GeV];FatJets",
                              100, minFatJetPt_, 1000 + minFatJetPt_);
    hFatJetEta_ = fs->make<TH1F>("hFatJetEta", "FatJet #eta;#eta;FatJets",
                               100, -5, 5);
    hFatJetPhi_ = fs->make<TH1F>("hFatJetPhi", "FatJet #phi;#phi;FatJets",
                               100, -3.1416, 3.1416);
    hFatJetMass_ = fs->make<TH1F>("hFatJetMass", "FatJet mass;Mass [GeV];FatJets",
                                100, 0, 500);
    hFatJetNConstituents_ = fs->make<TH1F>("hFatJetNConstituents", "FatJet constituents;N_{constituents};FatJets",
                                         100, 0, 200);

    hFatJetEtaVsPhi_ = fs->make<TH2F>("hFatJetEtaVsPhi", "FatJet #eta vs #phi;#eta;#phi",
                                    100, -5, 5, 100, -3.1416, 3.1416);
    hFatJetPtVsEta_ = fs->make<TH2F>("hFatJetPtVsEta", "FatJet p_{T} vs #eta;p_{T} [GeV];#eta",
                                   100, minFatJetPt_, 1000 + minFatJetPt_, 100, -5, 5);
    hFatJetConstituentPtVsEta_ = fs->make<TH2F>("hFatJetConstituentPtVsEta",
                                               "FatJet constituent p_{T} vs #eta;p_{T} [GeV];#eta",
                                               100, 0, 200, 100, -5, 5);
}

void FatJetAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    if (saveTree_) {
        fatjet_pt_.clear();
        fatjet_eta_.clear();
        fatjet_phi_.clear();
        fatjet_mass_.clear();
        fatjet_Idx_.clear();

        pf_pt_.clear();
        pf_eta_.clear();
        pf_phi_.clear();
        pf_IdxFatJet_.clear();
    }

    edm::Handle<reco::PFJetCollection> fatjets;
    iEvent.getByToken(fatjetsToken_, fatjets);

    if (!fatjets.isValid()) {
        // Use the stored fatjetsInputTag_ for logging
        edm::LogError("FatJetAnalyzer") << "Failed to retrieve FatJet collection: "
                                        << fatjetsInputTag_.encode();
        return;
    }

    int currentFatJetIndex = 0;

    for (const auto& fatjet : *fatjets) {
        if (fatjet.pt() < minFatJetPt_) continue;

        if (saveTree_) {
            fatjet_pt_.push_back(fatjet.pt());
            fatjet_eta_.push_back(fatjet.eta());
            fatjet_phi_.push_back(fatjet.phi());
            fatjet_mass_.push_back(fatjet.mass());
            fatjet_Idx_.push_back(currentFatJetIndex);

            // Corrected loop for constituents: use edm::Ptr<reco::PFCandidate>
            for (const edm::Ptr<reco::PFCandidate>& constituentPtr : fatjet.getPFConstituents()) {
                if (constituentPtr.isNull()) continue;

                pf_pt_.push_back(constituentPtr->pt());
                pf_eta_.push_back(constituentPtr->eta());
                pf_phi_.push_back(constituentPtr->phi());
                pf_IdxFatJet_.push_back(currentFatJetIndex);
            }
        }

        if (saveHistograms_) {
            hFatJetPt_->Fill(fatjet.pt());
            hFatJetEta_->Fill(fatjet.eta());
            hFatJetPhi_->Fill(fatjet.phi());
            hFatJetMass_->Fill(fatjet.mass());
            hFatJetNConstituents_->Fill(fatjet.getPFConstituents().size());
            hFatJetEtaVsPhi_->Fill(fatjet.eta(), fatjet.phi());
            hFatJetPtVsEta_->Fill(fatjet.pt(), fatjet.eta());

            // Corrected loop for constituents here as well
            for (const edm::Ptr<reco::PFCandidate>& constituentPtr : fatjet.getPFConstituents()) {
                if (constituentPtr.isNull()) continue;
                hFatJetConstituentPtVsEta_->Fill(constituentPtr->pt(), constituentPtr->eta());
            }
        }
        currentFatJetIndex++;
    }

    if (saveTree_ && !fatjet_pt_.empty()) {
        eventTree_->Fill();
    }
}

void FatJetAnalyzer::endJob() {
}

DEFINE_FWK_MODULE(FatJetAnalyzer);