#include "stdafx.h"
#include "ExposeActivationStateMachine.h"
#include "graphics/RenderFrame.h"

#include "RenderState.h"

const double startAngle = 5 * M_PI/4.0;
const double endAngle = startAngle + 2 * (M_PI/4.0);
const int numItems = 1;
const Color UNSELECTED_COLOR(0.4f, 0.425f, 0.45f, 0.75f);
const Color SELECTED_COLOR(0.505f, 0.831f, 0.114f, 0.95f);
const Color GOAL_COLOR(0.505f, 0.831f, 0.114f, 1.0f);

ExposeActivationStateMachine::ExposeActivationStateMachine() :
m_state(State::INACTIVE)
{
}


ExposeActivationStateMachine::~ExposeActivationStateMachine() { }

void ExposeActivationStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void ExposeActivationStateMachine::AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }
  
  switch( m_state )
  {
    case State::INACTIVE:
      if(appState == OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_state = State::INACTIVE;
      }
      break;
    case State::COMPLETE:
      if(appState != OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_state = State::INACTIVE;
      }
      break;
    case State::FINAL:
    default:
      break;
  }
  
  // State Loops
  switch (m_state) {
    case State::INACTIVE:
      break;
    case State::ACTIVE:
    {
    }
    case State::FINAL:
    default:
      break;
  }
}

void ExposeActivationStateMachine::resolveSelection() {
  m_stateChangeEvent(&OSCStateChangeEvent::RequestTransition)(OSCState::EXPOSE_FOCUSED);
}

void ExposeActivationStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {

}

void ExposeActivationStateMachine::Render(const RenderFrame &renderFrame) const  {
  if (m_state == State::ACTIVE || m_state == State::COMPLETE) {
  }
}

