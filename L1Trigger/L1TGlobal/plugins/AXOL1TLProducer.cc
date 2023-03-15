////
/// \class l1t::AnomalyDetectionAEProducer
///
/// Description: Create input for anomaly detection autoencoder model inference in the proper structure. 
///
///
/// \author: M. Quinnan UCSD
///
///  Modeled after BXVectorInputProducer.cc
///

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//#include <vector>
#include "DataFormats/L1Trigger/interface/BXVector.h"

#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/EtSum.h"
#include "DataFormats/L1TGlobal/interface/GlobalExtBlk.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"

#include "TMath.h"

using namespace std;
using namespace edm;

//HLS4ML compiled emulator modeling
#include <string>
#include "ap_fixed.h"
#include "hls4ml/emulator.h"

// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

namespace l1t {

  //
  // class declaration
  //
  //edm?

  //not sure what should be before EDProducer. stream, edm, one...?
  class AXOL1TLProducer : public one::EDProducer<> {
  public:
    explicit AXOL1TLProducer(const ParameterSet&);
    ~AXOL1TLProducer() override;
 
    static void fillDescriptions(ConfigurationDescriptions& descriptions);

  private:
    void produce(Event&, EventSetup const&) override;
    void beginJob() override; //not sure if need
    void endJob() override;
    //is this ok to not have beginRun and endRun? complained about no override
    // void beginRun(Run const& iR, EventSetup const& iE) override;
    // void endRun(Run const& iR, EventSetup const& iE) override;

    // ----------member data ---------------------------

    // BX parameters (needed?)
    int bxFirst_;
    int bxLast_;

    //number of inputs to store
    unsigned int maxNumMuCands_; //4
    unsigned int maxNumJetCands_; //10
    unsigned int maxNumEGCands_; //4
    //    unsigned int maxNumETCands_; //1

    // Tokens for inputs from other parts of the L1 system
    edm::EDGetToken egToken;
    edm::EDGetToken muToken;
    edm::EDGetToken jetToken;
    edm::EDGetToken etsumToken;

    // particle vectors
    int counter_;
    std::vector<l1t::Muon> muonVec;
    std::vector<l1t::EGamma> egammaVec;
    std::vector<l1t::Jet> jetVec;
    std::vector<l1t::EtSum> etsumVec;

    //HLS4ML emulator objects
    hls4mlEmulator::ModelLoader loader;
    std::shared_ptr<hls4mlEmulator::Model> model;
  };

  //
  // constructors and destructor
  //
  //AXOL1TLProducer::AXOL1TLProducer(const ParameterSet& iConfig) {
  // AXOL1TLProducer::AXOL1TLProducer(const ParameterSet& iConfig : loader(hls4mlEmulator::ModelLoader("GTADModel_v1")) ){
  //   egToken = consumes<BXVector<l1t::EGamma>>(iConfig.getParameter<InputTag>("egInputTag"));
  //   muToken = consumes<BXVector<l1t::Muon>>(iConfig.getParameter<InputTag>("muInputTag"));
  //   jetToken = consumes<BXVector<l1t::Jet>>(iConfig.getParameter<InputTag>("jetInputTag"));
  //   etsumToken = consumes<BXVector<l1t::EtSum>>(iConfig.getParameter<InputTag>("etsumInputTag"));

  AXOL1TLProducer::AXOL1TLProducer(const ParameterSet& iConfig){

    bxFirst_ = iConfig.getParameter<int>("bxFirst"); //needed?
    bxLast_ = iConfig.getParameter<int>("bxLast");
    
    egToken = consumes<BXVector<l1t::EGamma>>(iConfig.getParameter<InputTag>("egInputTag"));
    muToken = consumes<BXVector<l1t::Muon>>(iConfig.getParameter<InputTag>("muInputTag"));
    jetToken = consumes<BXVector<l1t::Jet>>(iConfig.getParameter<InputTag>("jetInputTag"));
    etsumToken = consumes<BXVector<l1t::EtSum>>(iConfig.getParameter<InputTag>("etsumInputTag"));

    // register what you produce
    produces<BXVector<l1t::EGamma>>();
    produces<BXVector<l1t::Muon>>();
    produces<BXVector<l1t::Jet>>();
    produces<BXVector<l1t::Tau>>();
    produces<BXVector<l1t::EtSum>>();

    //AE model and output
    //loader = ModelLoader(((std::string)std::getenv("CMSSW_BASE")).append(iConfig.getParameter<string>("compiledAnomalyModelLocation"))) //check syntax
    // uses compiledAnomalyModelLocation = cms.string("/src/L1Trigger/L1TCaloLayer1/data/compiledADModel/caloADModel_v1") in L1Trigger/L1TCaloLayer1/python/uct2016EmulatorDigis_cfi.py
    // loader = hls4mlEmulator::ModelLoader(iConfig.getParameter<string>("AXOL1TLModelVersion")); //check this works, need to define AXOL1TLModelVersion
    // std::string modelname = "/src/L1Trigger/L1TGlobal/test/GTADModel_v1"
    // loader = hls4mlEmulator::ModelLoader(modelname); //temp without ext repo

    //; //uses ext repo
    std::string modelname = "GTADModel_v1";
    loader = hls4mlEmulator::ModelLoader(modelname);
    model = loader.load_model();
    produces<float>("anomaly_score");

    // Setup parameters

    maxNumMuCands_   = iConfig.getParameter<int>("maxMuCand");
    maxNumJetCands_  = iConfig.getParameter<int>("maxJetCand");
    maxNumEGCands_   = iConfig.getParameter<int>("maxEGCand");
    // maxNumParticles_ = iConfig.getParameter<int>("maxParticles");
    // NumNNinputs_     = iConfig.getParameter<int>("NumNNinputs");
    // NumNNoutputs_    = iConfig.getParameter<int>("NumNNoutputs");

  }

  AXOL1TLProducer::~AXOL1TLProducer() {
    // //delete model
    // loader.destroy_model();
  }

  //
  // member functions
  //

  // ------------ method called to produce the data ------------
  void AXOL1TLProducer::produce(Event& iEvent, const EventSetup& iSetup) {

    LogDebug("l1t|Global") << "AXOL1TLProducer::produce function called...\n";

    //only keep up to # of objects
    int maxNumInMus = 4; //max 4 muons input to NN
    int maxNumInJets = 10; //max 10 jets input to NN
    int maxNumInEGs = 4; //max 4 egammas input to NN

    // Setup vectors
    std::vector<l1t::Muon> muonVec;
    std::vector<l1t::EGamma> egammaVec;
    std::vector<l1t::Jet> jetVec;
    std::vector<l1t::EtSum> etsumVec;

    //outputs
    std::unique_ptr<float> anomaly_score(new float); //store anomaly score

    //needed?
    int bxFirst = bxFirst_;
    int bxLast = bxLast_;
    std::unique_ptr<l1t::EGammaBxCollection> egammas(new l1t::EGammaBxCollection(0, bxFirst, bxLast));
    std::unique_ptr<l1t::MuonBxCollection> muons(new l1t::MuonBxCollection(0, bxFirst, bxLast));
    std::unique_ptr<l1t::JetBxCollection> jets(new l1t::JetBxCollection(0, bxFirst, bxLast));
    std::unique_ptr<l1t::EtSumBxCollection> etsums(new l1t::EtSumBxCollection(0, bxFirst, bxLast));
 
    // Bx to use...grab only bx=0 for now
    int bx = 0;

    // Input and output of  the model is in the input_t format as defined in the model's firmware/defines.h
    // ap_fixed<8, 6, AP_RND_CONV, AP_SAT> precompiledModelInput[57];
    ap_fixed<8, 6, AP_RND_CONV, AP_SAT> ADModelInput[57];
    // ap_fixed<10, 7> ADModelResult[13]; //pre-loss-computation result
    // ap_ufixed<18, 14> ADModelScore; //post-loss-computation result
    std::pair<std::array<ap_fixed<10, 7>, 13>, ap_ufixed<18, 14> > ADModelResult;

    // Make sure that you can get input EG
    Handle<BXVector<l1t::EGamma>> inputEgammas;
    if (iEvent.getByToken(egToken, inputEgammas)) {
      for (std::vector<l1t::EGamma>::const_iterator eg = inputEgammas->begin(bx); eg != inputEgammas->end(bx); ++eg) {
        if ( egammaVec.size() < maxNumEGCands_) { //note no threshold as in BXVector Producer
          egammaVec.push_back((*eg));
        }
      }
    } else {
      LogTrace("l1t|Global") << ">>> input EG collection not found!" << std::endl;
    }

    // Make sure that you can get input Muons
    Handle<BXVector<l1t::Muon>> inputMuons;
    if (iEvent.getByToken(muToken, inputMuons)) {
      for (std::vector<l1t::Muon>::const_iterator mu = inputMuons->begin(bx); mu != inputMuons->end(bx); ++mu) {
        if ( muonVec.size() < maxNumMuCands_) {
          muonVec.push_back((*mu));
        }
      }
    } else {
      LogTrace("l1t|Global") << ">>> input Mu collection not found!" << std::endl;
    }

    // Make sure that you can get input jet
    Handle<BXVector<l1t::Jet>> inputJets;
    if (iEvent.getByToken(jetToken, inputJets)) {
      for (std::vector<l1t::Jet>::const_iterator jet = inputJets->begin(bx); jet != inputJets->end(bx); ++jet) {
        if ( jetVec.size() < maxNumJetCands_) {
          jetVec.push_back((*jet));
        }
      }
    } else {
      LogTrace("l1t|Global") << ">>> input jet collection not found!" << std::endl;
    }

    // Make sure that you can get input etsum
    Handle<BXVector<l1t::EtSum>> inputEtsums;
    if (iEvent.getByToken(etsumToken, inputEtsums)) {
      for (std::vector<l1t::EtSum>::const_iterator etsum = inputEtsums->begin(bx); etsum != inputEtsums->end(bx);
           ++etsum) {
        etsumVec.push_back((*etsum));
      }
    } else {
      LogTrace("l1t|Global") << ">>> input etsum collection not found!" << std::endl;
    }

    //////////////////
    // Insert all the bx into the L1 Collections
    //printf("Event %i  EmptyBxEvt %i emptyBxTrailer %i diff %i \n",eventCnt_,emptyBxEvt_,emptyBxTrailer_,(emptyBxEvt_ - eventCnt_));

    //ADModelInput = [EtSum.et(), EtSum.eta(), EtSum.phi(), 
    //                4 egammas *(egamma_i.pt(), egamma_i.eta(), egamma_i.phi()),
    //                4 muons *(muon_i.pt(), muon_i.eta(), muon_i.phi()),
    //                10 jets *(jpt_i.et(), jet_i.eta(), jet_i.phi()),  ]


    //counter of starting index of next loop 
    int starti = 3;

    // Fill Etsums
    // for (int iETsum = 0; iETsum < int(maxNumETCands_); iETsum++) {
    for (int iETsum = 0; iETsum < int(1); iETsum++) {
      etsums->push_back(0, etsumVec[iETsum]);
     //This should fill the tensor in the proper order to be fed to the anomaly model
      ADModelInput[0] = iETsum.et();
      ADModelInput[1] = 0.0; //iETsum.eta();
      ADModelInput[2] = iETsum.phi();
    }

    // Fill Egammas
    for (int iEG = 0; iEG < int(maxNumEGCands_); iEG++) {
      if (iEG < maxNumInEGs) {
	egammas->push_back(0, egammaVec[iEG]);
	ADModelInput[starti+(3*iEG)+0] = iEG.et(); //starti=3
	ADModelInput[starti+(3*iEG)+1] = iEG.eta();
	ADModelInput[starti+(3*iEG)+2] = iEG.phi();
      }
    }
    starti += 3*int(maxNumInEGs); //start next loop at index 15 for example = 3*4egcands+3 = 15

    // Fill Muons
    for (int iMu = 0; iMu < int(maxNumMuCands_); iMu++) {
      if (iMu < maxNumInMus) {
	muons->push_back(0, muonVec[iMu]);
	ADModelInput[starti+(3*iMu)+0] = iMu.pt(); //starti = 15 
	ADModelInput[starti+(3*iMu)+1] = iMu.eta();
	ADModelInput[starti+(3*iMu)+2] = iMu.phi();      
      }
    }
    starti += 3*int(maxNumInMus)+3; //update starti again

    // Fill Jets
    for (int iJet = 0; iJet < int(maxNumJetCands_); iJet++) {
      if (iJet < maxNumInJets) {
	jets->push_back(0, jetVec[iJet]);
	ADModelInput[starti+(3*iJet)+0] = iJet.et(); 
	ADModelInput[starti+(3*iJet)+1] = iJet.eta();
	ADModelInput[starti+(3*iJet)+2] = iJet.phi();     
      }
    }

    // run inference on anomaly model
    model->prepare_input(ADModelInput); //scaling internal here
    model->predict();
    model->read_result(ADModelResult);// this should be the square sum model result 
    
    // ADModelScore[0] = model->computeLoss(ADModelResult);  now inside of readResult
    *anomaly_score = (ADModelResult.second).to_float();  //convert the fixed precision result to a proper c++ floating point

    iEvent.put(std::move(egammas));
    iEvent.put(std::move(muons));
    iEvent.put(std::move(jets));
    iEvent.put(std::move(etsums));
    iEvent.put(std::move(anomaly_score));

  }

  // ------------ method called once each job just before starting event loop ------------
  void AXOL1TLProducer::beginJob() {}

  // ------------ method called once each job just after ending the event loop ------------
  void AXOL1TLProducer::endJob() {}

  // // ------------ method called when starting to processes a run ------------

  // void AXOL1TLProducer::beginRun(Run const& iR, EventSetup const& iE) {
  //   LogDebug("GtAXOL1TLProducer") << "AXOL1TLProducer::beginRun function called...\n";

  //   counter_ = 0;
  // }

  // // ------------ method called when ending the processing of a run ------------
  // void AXOL1TLProducer::endRun(Run const& iR, EventSetup const& iE) {}



  // ------------ method fills 'descriptions' with the allowed parameters for the module ------------
  void AXOL1TLProducer::fillDescriptions(ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    ParameterSetDescription desc;
    // desc.add<bool>("setBptxMinus", true); 
    desc.setUnknown();
    descriptions.addDefault(desc);
  }

  }  // namespace l1t

//define this as a plug-in
DEFINE_FWK_MODULE(l1t::AXOL1TLProducer);
