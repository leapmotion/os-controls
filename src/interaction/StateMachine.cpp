#include "stdafx.h"
#include "StateMachine.h"
#include "Color.h"

StateMachine::StateMachine(void):
ContextMember("StateMachine"),
m_state(OSCState::BASE),
m_cursorView(15.0f, Color(1.0f, 1.0f, 1.0f, 0.0f)),
m_mediaView(Vector3(300, 300, 0), 5.0f)
{
}

StateMachine::~StateMachine(void)
{
}

// Transition Checking Loop
void StateMachine::AutoFilter(std::shared_ptr<Leap::Hand> pHand, const HandPose handPose, OSCState& state) {
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
    case HandPose::FiveFingers:
    default:
      break;
  }

  if(desiredState == OSCState::BASE || m_state == OSCState::BASE)
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
}

void StateMachine::OnHandVanished() {
  m_state = OSCState::FINAL;
}

// Distpatch Loop
void StateMachine::Tick(std::chrono::duration<double> deltaT) {
  if(m_state == OSCState::FINAL && !m_mediaView->IsVisible()) {
    m_context.reset();
    return;
  }
}

