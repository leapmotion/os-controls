#include "stdafx.h"
#include "StateMachine.h"
#include "Color.h"
#include "ExposeViewStateMachine.h"
#include "InteractionConfigs.h"
#include "osinterface/OSCursor.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/OSWindow.h"
#include "utility/NativeWindow.h"

StateMachine::StateMachine(void):
  ContextMember("StateMachine"),
  m_state(OSCState::BASE),
  m_ppmm(96.0f/25.4f),
  m_scrollState(ScrollState::DECAYING),
  m_lastScrollReleaseTimestep(0.0f),
  m_handDelta(0.0,0.0),
  m_scrollOperation(nullptr)
{
}

StateMachine::~StateMachine(void)
{
  m_scrollOperation.reset();
}

// Transition Checking Loop
void StateMachine::AutoFilter(std::shared_ptr<Leap::Hand> pHand, const FrameTime& frameTime, const HandPose& handPose, const HandPinch& handPinch, const HandLocation& handLocation, const Scroll& scroll, OSCState& state, ScrollState& scrollState) {

  std::lock_guard<std::mutex> lk(m_lock);
  if(m_state == OSCState::FINAL) {
    return;
  }
  
  m_lastScrollReleaseTimestep += frameTime.deltaTime;
  
  if ( m_lastScrollReleaseTimestep > 1000000 && m_scrollState != ScrollState::ACTIVE ) {
    // Map the hand pose to a candidate media control state
    auto desiredState = OSCState::BASE;
    switch(handPose) {
      case HandPose::ZeroFingers:
        desiredState = OSCState::BASE;
        break;
      case HandPose::OneFinger:
        desiredState = OSCState::MEDIA_MENU_FOCUSED;
        break;
      case HandPose::TwoFingers:
        desiredState = OSCState::BASE;
        break;
      case HandPose::ThreeFingers:
        desiredState = OSCState::DESKTOP_SWITCHER_FOCUSED;
        break;
      case HandPose::FourFingers:
        desiredState = OSCState::EXPOSE_FOCUSED;
        break;
      case HandPose::Clawed:
        desiredState = OSCState::MEDIA_MENU_FOCUSED;
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
  }
  
  // Ok, we've got a decision about what state we're in now.  Report it back to the user.
  state = m_state;
  
  scrollState = m_scrollState; //in case we don't change state

  Vector2 deltaScroll;

#if USE_HAND_SCROLL
  const double deltaScrollMultiplier = 0.15;
  const double deltaScrollThreshold = 0.15;
  deltaScroll = -deltaScrollMultiplier*scroll.m_deltaScrollMM.head<2>();
  switch (m_scrollState) {
  case ScrollState::ACTIVE:
    if (deltaScroll.squaredNorm() < deltaScrollThreshold) {
      m_scrollOperation.reset();
      scrollState = ScrollState::DECAYING;
    }
    break;
  case ScrollState::DECAYING:
    if (deltaScroll.squaredNorm() > deltaScrollThreshold) {
      AutowiredFast<OSCursor> cursor;
      if (cursor) {
        auto screenPosition = handLocation.screenPosition();
        OSPoint point{ static_cast<float>(screenPosition.x()), static_cast<float>(screenPosition.y()) };
        // Set the current cursor position
        cursor->SetCursorPos(point);
        // Make the application at the point become active
        NativeWindow::RaiseWindowAtPosition(point.x, point.y);
      }
      m_scrollOperation = m_windowScroller->BeginScroll();
      if (m_scrollOperation){
        scrollState = ScrollState::ACTIVE;
      }
    }
    break;
  }
#else
  switch(m_scrollState) {
  case ScrollState::ACTIVE:
    if(!handPinch.isPinching) {
      m_scrollOperation.reset();
        m_lastScrollReleaseTimestep = 0.0f;
      scrollState = ScrollState::DECAYING;
    }
    break;
  case ScrollState::DECAYING:
      if ( handPinch.isPinching && m_state == OSCState::BASE ) {
      AutowiredFast<OSCursor> cursor;
      if(cursor) {
        auto screenPosition = handLocation.screenPosition();
        OSPoint point{static_cast<float>(screenPosition.x()), static_cast<float>(screenPosition.y())};

        // Move cursor
        AutowiredFast<OSCursor> cursor;
        if(cursor) {
          // Set the current cursor position
          cursor->SetCursorPos(point);
          // Make the application at the point become active
          NativeWindow::RaiseWindowAtPosition(point.x, point.y);
        }

        // Update the pixels-per-inch for scrolling on this screen
        float ppi = 96.0f;
        AutowiredFast<OSVirtualScreen> virtualScreen;
        if(virtualScreen) {
          OSScreen screen = virtualScreen->ClosestScreen(point);
          ppi = screen.PixelsPerInch();
        }
        m_ppmm = ppi / 25.4f;

        m_scrollOperation = m_windowScroller->BeginScroll();
        if(m_scrollOperation) {
          scrollState = ScrollState::ACTIVE;
        }
      }
      deltaScroll = Vector2{handLocation.dmmX, handLocation.dmmY};
      break;
    }
  }
#endif
  
  m_handDelta = deltaScroll;
  m_scrollState = scrollState;
}

void StateMachine::OnHandVanished() {
  std::lock_guard<std::mutex> lk(m_lock);
  m_state = OSCState::FINAL;
  m_scrollState = ScrollState::DECAYING;
  m_scrollOperation.reset();
}

// Distpatch Loop
void StateMachine::Tick(std::chrono::duration<double> deltaT) {
  std::lock_guard<std::mutex> lk(m_lock);

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
      m_scrollOperation->ScrollBy(0.0f, (float)m_handDelta.y() * SCROLL_SENSITIVITY * m_ppmm);
      break;
    case ScrollState::DECAYING:
      break;
    default:
      break;
  }

  m_handDelta = Vector2(0, 0);
}
