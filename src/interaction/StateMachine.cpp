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
void StateMachine::AutoFilter(Leap::Hand* pHand, const HandLocation& handLocation, const HandPose handPose, OSCState& state) {
  if(!pHand) {
    // Transition to this state unconditionally and short-circuit
    m_state = OSCState::Final;
    return;
  }
  
  state = m_state;
}

// Distpatch Loop
void StateMachine::Update(std::chrono::duration<double> deltaT) {
  if(m_state == OSCState::Final) {
    m_context.reset();
    return;
  }
}

