/**
 * \class CorrCondition
 *
 *
 * Description: evaluation of a correlation condition.
 *
 * Implementation:
 *    <TODO: enter implementation details>
 *
 *\new features: Dragana Pilipovic
 *                - updated for invariant mass over delta R condition*
 */

// this class header
#include "L1Trigger/L1TGlobal/interface/CorrCondition.h"

// system include files
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <algorithm>

// user include files
//   base classes
#include "L1Trigger/L1TGlobal/interface/AXOL1TLTemplate.h"  //new
#include "L1Trigger/L1TGlobal/interface/ConditionEvaluation.h"

#include "L1Trigger/L1TGlobal/interface/MuCondition.h"
#include "L1Trigger/L1TGlobal/interface/AXOL1TLCondition.h"  //new
#include "L1Trigger/L1TGlobal/interface/CaloCondition.h"
#include "L1Trigger/L1TGlobal/interface/EnergySumCondition.h"
#include "L1Trigger/L1TGlobal/interface/MuonTemplate.h"
#include "L1Trigger/L1TGlobal/interface/CaloTemplate.h"
#include "L1Trigger/L1TGlobal/interface/EnergySumTemplate.h"
#include "L1Trigger/L1TGlobal/interface/GlobalScales.h"

#include "DataFormats/L1Trigger/interface/L1Candidate.h"

#include "L1Trigger/L1TGlobal/interface/GlobalBoard.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/MessageLogger/interface/MessageDrop.h"

// constructors
//     default
l1t::AXOL1TLCondition::AXOL1TLCondition() : ConditionEvaluation() {
  // empty
}

//     from base template condition (from event setup usually)
l1t::AXOL1TLCondition::AXOL1TLCondition(const GlobalCondition* axol1tlTemplate, const GlobalBoard* ptrGTB)
    : ConditionEvaluation(),
      m_gtAXOL1TLTemplate(static_cast<const AXOL1TLTemplate*>(axol1tlTemplate)),
      m_gtGTB(ptrGTB) {}

// copy constructor
void l1t::AXOL1TLCondition::copy(const l1t::AXOL1TLCondition& cp) {
  m_gtAXOL1TLTemplate = cp.gtAXOL1TLTemplate();
  m_gtGTB = cp.gtGTB();

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
void l1t::AXOL1TLCondition::setGtAXOL1TLTemplate(const AXOL1TLTemplate* caloTempl) { m_gtAXOL1TLTemplate = caloTempl; }

///   set the pointer to uGT GlobalBoard
void l1t::AXOL1TLCondition::setuGtB(const GlobalBoard* ptrGTB) { m_gtGTB = ptrGTB; }

const bool l1t::AXOL1TLCondition::evaluateCondition(const int bxEval) const {
  bool condResult = false;
  return condResult;
}

const bool l1t::AXOL1TLCondition::checkObjectParameter(const int iCondition, const l1t::L1Candidate& cand) const {
  return true;
}

void l1t::AXOL1TLCondition::print(std::ostream& myCout) const {
  myCout << "Dummy Print for AXOL1TLCondition" << std::endl;
  m_gtAXOL1TLTemplate->print(myCout);

  ConditionEvaluation::print(myCout);
}
