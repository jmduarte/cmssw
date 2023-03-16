import FWCore.ParameterSet.Config as cms

AXOL1TLProducer = cms.EDProducer(
    "AXOL1TLProducer",
    egToken = cms.untracked.InputTag("caloStage2Digis","egInputTag"),
    jetToken = cms.untracked.InputTag("caloStage2Digis","jetInputTag"),
    muonToken = cms.untracked.InputTag("gmtStage2Digis","muInputTag"),
    etsumToken = cms.untracked.InputTag("caloStage2Digis","etsumInputTag"),
#    anomalyModelLocation = cms.string("/src/L1Trigger/anomalyTriggerSkunkworks/data/uGTADModel/uGTModel/"),
)


