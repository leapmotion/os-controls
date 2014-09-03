#include "stdafx.h"
#include "StateMachine.h"
#include "Color.h"
#include "ExposeViewProxy.h"
#include "osinterface/OSCursor.h"

StateMachine::StateMachine(void):
  ContextMember("StateMachine"),
  m_state(OSCState::BASE),
  m_scrollState(ScrollState::DECAYING),
  m_scrollOperation(nullptr),
  m_handDelta(0,0),
  m_cursorView(15.0f, Color(1.0f, 1.0f, 1.0f, 0.0f))
{
}

StateMachine::~StateMachine(void)
{
  m_scrollOperation.reset();
}

// Transition Checking Loop
void StateMachine::AutoFilter(std::shared_ptr<Leap::Hand> pHand, const HandPose handPose, const HandPinch& handPinch, const HandLocation& handLocation, OSCState& state, ScrollState& scrollState) {
  if (m_state == OSCState::FINAL) {
    return;
  }
  
  // Map the hand pose to a candidate media control state
  auto desiredState = OSCState::BASE;
  switch(handPose) {
    case HandPose::ZeroFingers:
      desiredState = OSCState::BASE;
      break;
    case HandPose::OneFinger:
      desiredState = OSCState::BASE;
      break;
    case HandPose::TwoFingers:
      desiredState = OSCState::MEDIA_MENU_FOCUSED;
      break;
    case HandPose::ThreeFingers:
      desiredState = OSCState::DESKTOP_SWITCHER_FOCUSED;
      break;
    case HandPose::FourFingers:
      desiredState = OSCState::EXPOSE_FOCUSED;
      break;
    case HandPose::FiveFingers:
    default:
      break;
  }

  //if(desiredState == OSCState::BASE || m_state == OSCState::BASE)
    // If we want to go to the base state, then transition there.  Just do it, don't think
    // about it, do it.  Views in the this context all have their own states and know where
    // they are, they'll be able to tell that we're in the Base state and that they should
    // probably take steps to hide themselves.
    //
    // This means that a user who quickly changes from one finger to two fingers without
    // going through the ground case will actually not cause a menu change to happen, and
    // if this isn't the desired behavior, then change it by assigning the current state
    // unconditionally!
  m_state = desiredState;
  
  // Ok, we've got a decision about what state we're in now.  Report it back to the user.
  state = m_state;
  
  scrollState = m_scrollState; //in case we don't change state
  
  switch ( m_scrollState ) {
    case ScrollState::ACTIVE:
      if ( !handPinch.isPinching ) {
        m_scrollOperation.reset();
        scrollState = ScrollState::DECAYING;
      }
      break;
    case ScrollState::DECAYING:
      if ( handPinch.isPinching ) {
        AutowiredFast<OSCursor> cursor;
        if (cursor) {
          auto screenPosition = handLocation.screenPosition();
          OSPoint point{static_cast<float>(screenPosition.x()), static_cast<float>(screenPosition.y())};
          // Set the current cursor position
          cursor->SetCursorPos(point);
          // Make the application at the point become active
          // FIXME
        }
        m_scrollOperation = m_windowScroller->BeginScroll();
        if(m_scrollOperation){
          scrollState = ScrollState::ACTIVE;
        }
      }
      break;
  }
  
  m_handDelta =  Vector2(handLocation.dX, handLocation.dY);
  m_scrollState = scrollState;
}

void StateMachine::OnHandVanished() {
  m_state = OSCState::FINAL;
}

// Distpatch Loop
void StateMachine::Tick(std::chrono::duration<double> deltaT) {
  switch ( m_state ) {
    case OSCState::FINAL:
      // Remove our controls from the scene graph
      m_mediaViewStateMachine->RemoveFromParent();
      m_cursorView->RemoveFromParent();
      // Shutdown the context
      m_context->SignalShutdown();
      // Remove our own reference to the context
      m_context.reset();
      return;
    default:
      break;
  }
  
  switch ( m_scrollState ) {
    case ScrollState::ACTIVE:
      m_scrollOperation->ScrollBy(0.0f, m_handDelta.y());
      break;
    case ScrollState::DECAYING:
      break;
    default:
      break;
  }
}

