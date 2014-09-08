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
  std::cout << "AppState: " << static_cast<int>(appState) << std::endl;
  std::cout << "ExposeViewState: " << static_cast<int>(m_state) << std::endl;
  State desiredState = m_state;
  //State Transitions
  switch (m_state) {
    case State::INACTIVE:
      //If the context wants to move into expose mode, move into attempting to aquire the lock.
      if(appState == OSCState::EXPOSE_FOCUSED) {
        desiredState = State::AWAITING_LOCK;
      }
      break;
    case State::AWAITING_LOCK:
      //Make sure we still want to move into expose mode if not become inactive
      if(appState != OSCState::EXPOSE_FOCUSED) {
        desiredState = State::INACTIVE;
        break;
      }
      
      //Try and aquire a reference to exposeView and move into the view
      m_exposeView = m_exposeViewAccessManager->Lock();
      
      if( m_exposeView != nullptr) {
        m_exposeView->StartView();
        m_exposeView->GetContext()->Snoop(shared_from_this());
        desiredState = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      //If we need to release the view, tell the view to shut down then drop the reference.
      if(appState != OSCState::EXPOSE_FOCUSED) {
        m_exposeView.reset();
        desiredState = State::INACTIVE;
        break;
      }
      break;
    case State::COMPLETE:
      if(appState != OSCState::EXPOSE_FOCUSED) {
        m_exposeView.reset();
        desiredState = State::INACTIVE;
      }
      break;
  }
  
  m_state = desiredState;
  
  //State Loops
  switch (m_state) {
    case State::INACTIVE:
      //Make sure we're not holding a reference to expose view
      if(m_exposeView != nullptr) {
        m_exposeView.reset();
      }
      break;
    case State::AWAITING_LOCK:
      break;
    case State::ACTIVE:
      m_exposeView->SetHandData(handData);
      break;
    case State::COMPLETE:
      if(m_exposeView != nullptr) {
        m_exposeView.reset();
      }
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
  m_state = State::COMPLETE;
}

