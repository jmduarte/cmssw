#
# WARNING: This file is in the L1T configuration critical path.
#
# All changes must be explicitly discussed with the L1T offline coordinator.
#
import FWCore.ParameterSet.Config as cms

# axol1tl emulator

AXOL1TL_emulation = cms.EDProducer(
    "AXOL1TLProducer",
    egInputTag = cms.InputTag('simCaloStage2Digis'),
    muInputTag = cms.InputTag('simGmtStage2Digis'),
    jetInputTag = cms.InputTag('simCaloStage2Digis'),
    etsumInputTag = cms.InputTag('simCaloStage2Digis'),
    AXOL1TLModelVersion = cms.string('GTADModel_v1')
)
