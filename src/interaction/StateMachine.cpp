#include "stdafx.h"
#include "StateMachine.h"

StateMachine::StateMachine(void):
  m_state(OSCState::Base)
{
}

StateMachine::~StateMachine(void)
{
  
}

// Transition Checking Loop
void StateMachine::AutoFilter(Leap::Hand* pHand, const HandLocation& handLocation, const HandPose handPose, OCSState& state) {
  if(!pHand) {
    // Transition to this state unconditionally and short-circuit
    m_state = OSCState::Final;
    return;
  }
  
  state = m_state;
}

// Distpatch Loop
void StateMachine::Update(double deltaT) {
  if(m_state == OCSState::Final) {
    m_context.reset();
    return;
  }
}

