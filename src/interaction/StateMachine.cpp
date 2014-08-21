#include "stdafx.h"
#include "StateMachine.h"

StateMachine::StateMachine(void):
  m_state(OSCState::Initial)
{
}

StateMachine::~StateMachine(void)
{
}

void StateMachine::AutoFilter(Leap::Hand* pHand) {
  if(!pHand) {
    // Transition to this state unconditionally and short-circuit
    m_state = OSCState::Final;
    return;
  }
}

void StateMachine::Update(double deltaT) {

}