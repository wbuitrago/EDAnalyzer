#ifndef FatJetAnalyzer_h
#define FatJetAnalyzer_h

#include <memory>
#include <vector>
#include <string>
#include "TTree.h"

// FWCore includes
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"

// Data formats
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/ValueMap.h"

class FatJetAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
    explicit FatJetAnalyzer(const edm::ParameterSet&);
    ~FatJetAnalyzer() override;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override;

    // Configuración
    const bool saveTree_;
    const double minFatJetPt_;
    const int signalLabel_;
    
    // InputTags almacenados para logging
    const edm::InputTag fatjetsTag_ps_;
    const edm::InputTag verticesTag_ps_;
    const edm::InputTag softDropMassMapTag_ps_;
    const edm::InputTag tau1MapTag_ps_;
    const edm::InputTag tau2MapTag_ps_;
    const edm::InputTag tau3MapTag_ps_;
    const edm::InputTag puppiWeightMapTag_ps_;      // NUEVO
    const edm::InputTag puppiWeightNoLepMapTag_ps_; // NUEVO

    // Tokens
    edm::EDGetTokenT<reco::PFJetCollection> fatjetsToken_;
    edm::EDGetTokenT<reco::VertexCollection> verticesToken_;
    edm::EDGetTokenT<edm::ValueMap<float>> softDropMassToken_; 
    edm::EDGetTokenT<edm::ValueMap<float>> tau1Token_;         
    edm::EDGetTokenT<edm::ValueMap<float>> tau2Token_;         
    edm::EDGetTokenT<edm::ValueMap<float>> tau3Token_;         
    edm::EDGetTokenT<edm::ValueMap<float>> puppiWeightToken_;      // NUEVO
    edm::EDGetTokenT<edm::ValueMap<float>> puppiWeightNoLepToken_; // NUEVO


    // TTree y ramas
    TTree* fatJetTree_;

    // Variables del FatJet
    float fj_pt_, fj_eta_, fj_phi_, fj_mass_;
    float fj_msoftdrop_;
    float fj_tau1_, fj_tau2_, fj_tau3_;
    int fj_nConstituents_;
    int fj_label_; 

    // Variables de los constituyentes (vectores)
    std::vector<float> pfc_pt_;
    std::vector<float> pfc_eta_;
    std::vector<float> pfc_phi_;
    std::vector<float> pfc_energy_;
    std::vector<float> pfc_mass_;
    std::vector<float> pfc_etarel_;
    std::vector<float> pfc_phirel_;
    std::vector<float> pfc_dxy_;
    std::vector<float> pfc_dz_;
    std::vector<float> pfc_dxy_error_;
    std::vector<float> pfc_dz_error_;
    std::vector<int> pfc_pdgId_;
    std::vector<int> pfc_charge_;
    std::vector<int> pfc_numberOfValidHits_;
    std::vector<float> pfc_normalizedChi2_;
    std::vector<float> pfc_puppiWeight_;        
    std::vector<float> pfc_puppiWeightNoLep_;  
};

#endif