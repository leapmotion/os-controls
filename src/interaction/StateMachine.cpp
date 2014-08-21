#include "stdafx.h"
#include "StateMachine.h"

StateMachine::StateMachine(void):
  m_state(OSCState::Base)
{
}

StateMachine::~StateMachine(void)
{
}

void StateMachine::AutoFilter(Leap::Hand* pHand, const HandLocation& handLocation, const HandPose handPose) {
  if(!pHand) {
    // Transition to this state unconditionally and short-circuit
    m_state = OSCState::Final;
    return;
  }
}

void StateMachine::Update(double deltaT) {

}