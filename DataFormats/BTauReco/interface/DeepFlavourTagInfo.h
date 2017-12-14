#ifndef DataFormats_BTauReco_DeepFlavourTagInfo_h
#define DataFormats_BTauReco_DeepFlavourTagInfo_h

#include "DataFormats/Common/interface/CMS_CLASS_VERSION.h"
#include "DataFormats/BTauReco/interface/BaseTagInfo.h"
#include "DataFormats/BTauReco/interface/FeaturesTagInfo.h"
#include "DataFormats/BTauReco/interface/DeepFlavourFeatures.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

namespace reco {

typedef  FeaturesTagInfo<btagbtvdeep::DeepFlavourFeatures> DeepFlavourTagInfo;

DECLARE_EDM_REFS( DeepFlavourTagInfo )

}

#endif // DataFormats_BTauReco_DeepFlavourTagInfo_h
