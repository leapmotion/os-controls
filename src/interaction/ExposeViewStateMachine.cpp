#include "stdafx.h"
#include "ExposeViewStateMachine.h"
#include "expose/ExposeView.h"
#include "expose/ExposeViewAccessManager.h"

ExposeViewStateMachine::ExposeViewStateMachine(void) :
m_state(State::INACTIVE)
{
  m_exposeView = nullptr;
}

ExposeViewStateMachine::~ExposeViewStateMachine(void) {
  
}

void ExposeViewStateMachine::AutoFilter(OSCState appState, const HandData& handData) {
  doStateTransitions(appState);
  doStateLoops(handData);
}

void ExposeViewStateMachine::doStateTransitions(OSCState appState) {
  if( m_state == State::INACTIVE &&
     appState == OSCState::EXPOSE_FOCUSED ) {
    m_state = State::AWAITING_LOCK;
  }
  else if ( m_state == State::AWAITING_LOCK ) {
    if( m_exposeView != nullptr ) {
      m_exposeView->StartView();
      m_exposeView->GetContext()->Snoop(shared_from_this());
      m_state = State::ACTIVE;
    }
    else if ( appState != OSCState::EXPOSE_FOCUSED ) {
      m_state = State::INACTIVE;
    }
  }
  else if ( m_state == State::ACTIVE &&
           appState != OSCState::EXPOSE_FOCUSED ) {
    m_exposeView.reset();
    m_state = State::INACTIVE;
  }
}

void ExposeViewStateMachine::doStateLoops(const HandData& handData) {
  switch (m_state) {
    case State::INACTIVE:
      //Make sure we're not holding a reference to expose view
      if(m_exposeView != nullptr) {
        m_exposeView.reset();
      }
      break;
    case State::AWAITING_LOCK:
      // Try and aquire a reference to exposeView and move into the view
      // This is a non-blocking call.
      m_exposeView = m_exposeViewAccessManager->Lock();
      break;
    case State::ACTIVE:
      // Update the ExposeView
      m_exposeView->SetHandData(handData);
      break;
    default:
      break;
  }
}

void ExposeViewStateMachine::Shutdown() {
  m_exposeView.reset();
  m_state = State::INACTIVE;
}

void ExposeViewStateMachine::applyUserInput(const HandLocation& handLocation) {
  
}

void ExposeViewStateMachine::onWindowSelected(ExposeViewWindow& osWindow)  {
  m_stateChangeEvent(&OSCStateChangeEvent::RequestTransition)(OSCState::BASE);
}

