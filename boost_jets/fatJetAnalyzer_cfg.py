import FWCore.ParameterSet.Config as cms

process = cms.Process("FATJETANALYSIS")

# MessageLogger setup
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

# Input files
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/eos/user/w/wbuitrag/Gridpacks/Local_Production/Boosted/VBSWLZLToJets_5f_LO_FXFX_200pT_Run3Summer23wmLHEGS/root/AOD/VBSWLZLToJets_5f_LO_FXFX_200pT.root'
    )
)

# Number of events to process
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

# FatJet analyzer configuration
process.fatJetAnalyzer = cms.EDAnalyzer('FatJetAnalyzer',
    saveHistograms = cms.bool(True),
    saveTree = cms.bool(True),
    minFatJetPt = cms.double(150.0),
    signalLabel = cms.int32(2)  # 1 = WL, 2 = WT
)

# Output file service
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("FatJetAnalyzer_WLZL.root")
)

# Path definition
process.p = cms.Path(process.fatJetAnalyzer)