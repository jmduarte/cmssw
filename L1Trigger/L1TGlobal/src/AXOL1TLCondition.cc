// this class header
#include "L1Trigger/L1TGlobal/interface/AXOL1TLCondition.h"

// system include files
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <algorithm>

// user include files
//   base classes
#include "L1Trigger/L1TGlobal/interface/AXOL1TLTemplate.h"
#include "L1Trigger/L1TGlobal/interface/ConditionEvaluation.h"

// #include "DataFormats/L1Trigger/interface/AXOL1TL.h"

#include "L1Trigger/L1TGlobal/interface/GlobalBoard.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/MessageLogger/interface/MessageDrop.h"

// constructors
//     default
l1t::AXOL1TLCondition::AXOL1TLCondition() : ConditionEvaluation() {
  // empty
}

//     from base template condition (from event setup usually)
l1t::AXOL1TLCondition::AXOL1TLCondition(const ExternalCondition* AX0L1TLTemplate,
					const GlobalBoard* ptrGTL//,
                                              // const int nrL1MuShower
					)
    : ConditionEvaluation(),
      m_gtAXOL1TLTemplate(static_cast<const AXOL1TLTemplate*>(AX0L1TLTemplate)),
}

// copy constructor
void l1t::AXOL1TLCondition::copy(const l1t::AXOL1TLCondition& cp) {
  m_gtAXOL1TLTemplate = cp.gtAXOL1TLTemplate();
  // m_gtGTL = cp.gtGTL();

  m_condMaxNumberObjects = cp.condMaxNumberObjects();
  m_condLastResult = cp.condLastResult();
  m_combinationsInCond = cp.getCombinationsInCond();

  m_verbosity = cp.m_verbosity;
}

l1t::AXOL1TLCondition::AXOL1TLCondition(const l1t::AXOL1TLCondition& cp) : ConditionEvaluation() { copy(cp); }

// destructor
l1t::AXOL1TLCondition::~AXOL1TLCondition() {
  // empty
}

// equal operator
l1t::AXOL1TLCondition& l1t::AXOL1TLCondition::operator=(const l1t::AXOL1TLCondition& cp) {
  copy(cp);
  return *this;
}

// methods
void l1t::AXOL1TLCondition::setGtAXOL1TLTemplate(const AXOL1TLTemplate* AX0L1TLTempl) {
  m_gtAXOL1TLTemplate = AX0L1TLTempl;
}

///   set the pointer to GTL
void l1t::AXOL1TLCondition::setGtGTL(const GlobalBoard* ptrGTL) { m_gtGTL = ptrGTL; }

// try all object permutations and check spatial correlations, if required
const bool l1t::AXOL1TLCondition::evaluateCondition(const int bxEval) const {

  return true; //dummy
}


void l1t::AXOL1TLCondition::print(std::ostream& myCout) const {
  m_gtAXOL1TLTemplate->print(myCout);

  ConditionEvaluation::print(myCout);
}
