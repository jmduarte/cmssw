#ifndef RecoBTag_DeepFlavour_ChargedCandidateConverter_h
#define RecoBTag_DeepFlavour_ChargedCandidateConverter_h

#include "RecoBTag/DeepFlavour/interface/deep_helpers.h"
#include "RecoBTag/DeepFlavour/interface/TrackInfoBuilder.h"
#include "DataFormats/BTauReco/interface/ChargedCandidateFeatures.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

namespace btagbtvdeep {
  
  template <typename CandidateType>
    void commonCandidateToFeatures(const CandidateType * c_pf,
				   const reco::Jet & jet,
				   const TrackInfoBuilder & track_info,
				   const float & drminpfcandsv, const float & jetR,
				   ChargedCandidateFeatures & c_pf_features) {
    
    c_pf_features.ptrel = catch_infs_and_bound(c_pf->pt()/jet.pt(),
					       0,-1,0,-1);
    
    c_pf_features.btagPf_trackEtaRel     =catch_infs_and_bound(track_info.getTrackEtaRel(),  0,-5,15);
    c_pf_features.btagPf_trackPtRel      =catch_infs_and_bound(track_info.getTrackPtRel(),   0,-1,4);
    c_pf_features.btagPf_trackPPar       =catch_infs_and_bound(track_info.getTrackPPar(),    0,-1e5,1e5 );
    c_pf_features.btagPf_trackDeltaR     =catch_infs_and_bound(track_info.getTrackDeltaR(),  0,-5,5 );
    c_pf_features.btagPf_trackPtRatio    =catch_infs_and_bound(track_info.getTrackPtRatio(), 0,-1,10);
    c_pf_features.btagPf_trackPParRatio  =catch_infs_and_bound(track_info.getTrackPParRatio(),0,-10,100);
    c_pf_features.btagPf_trackSip3dVal   =catch_infs_and_bound(track_info.getTrackSip3dVal(), 0, -1,1e5 );
    c_pf_features.btagPf_trackSip3dSig   =catch_infs_and_bound(track_info.getTrackSip3dSig(), 0, -1,4e4 );
    c_pf_features.btagPf_trackSip2dVal   =catch_infs_and_bound(track_info.getTrackSip2dVal(), 0, -1,70 );
    c_pf_features.btagPf_trackSip2dSig   =catch_infs_and_bound(track_info.getTrackSip2dSig(), 0, -1,4e4 );
    c_pf_features.btagPf_trackJetDistVal =catch_infs_and_bound(track_info.getTrackJetDistVal(),0,-20,1 );
    
    c_pf_features.drminsv = catch_infs_and_bound(drminpfcandsv,0,-1.*jetR,0,-1.*jetR);
    
  }
  
  void packedCandidateToFeatures(const pat::PackedCandidate * c_pf,
				 const pat::Jet & jet,
				 const TrackInfoBuilder & track_info,
				 const float drminpfcandsv, const float jetR, 
				 ChargedCandidateFeatures & c_pf_features) ;
  
  
  void recoCandidateToFeatures(const reco::PFCandidate * c_pf,
			       const reco::Jet & jet,
			       const TrackInfoBuilder & track_info,
			       const float drminpfcandsv, const float jetR, const float puppiw,
			       const int pv_ass_quality,
			       const reco::VertexRef & pv, 
			       ChargedCandidateFeatures & c_pf_features) ;
  
  
}

#endif //RecoBTag_DeepFlavour_ChargedCandidateConverter_h
