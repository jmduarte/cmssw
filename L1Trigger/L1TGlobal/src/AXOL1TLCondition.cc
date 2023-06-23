// constructors
//     default
l1t::AXOL1TLCondition::AXOL1TLCondition() : ConditionEvaluation() {
  // empty
}

//     from base template condition (from event setup usually)
l1t::AXOL1TLCondition::AXOL1TLCondition(const GlobalCondition* AXOL1TLTemplate,
					const GlobalBoard* ptrGTB)
    : ConditionEvaluation(),
      m_gtAXOL1TLTemplate(static_cast<const AXOL1TLTemplate*>(AXOL1TLTemplate)),
      m_gtGTL(ptrGTB) {}

// copy constructor
void l1t::AXOL1TLCondition::copy(const l1t::AXOL1TLCondition& cp) {
  m_AXOL1TLTemplate = cp.AXOL1TLTemplate();
  m_uGtB = cp.getuGtB();

  m_condMaxNumberObjects = cp.condMaxNumberObjects();
  m_condLastResult = cp.condLastResult();
  m_combinationsInCond = cp.getCombinationsInCond();

  m_verbosity = cp.m_verbosity;
}

l1t::AXOL1TLCondition::AXOL1TLCondition(const l1t::AXOL1TLCondition& cp) : ConditionEvaluation() { copy(cp); }

// destructor
l1t::AXOL1TLCondition::~AXOL1TLCondition() {
  // empty


