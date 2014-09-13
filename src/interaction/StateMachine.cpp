#include "stdafx.h"
#include "StateMachine.h"
#include "InteractionConfigs.h"
#include "expose/ExposeViewStateMachine.h"
#include "osinterface/OSCursor.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/OSWindow.h"
#include "utility/NativeWindow.h"

#include "Color.h"

StateMachine::StateMachine(void) :
  ContextMember("StateMachine"),
  m_state(OSCState::BASE),
  m_scrollType(ScrollType::PINCH_SCROLL),
  m_handDelta(0.0,0.0),
  m_lastHandLocation(0.0f,0.0f),
  m_lastScrollReleaseTimestep(0.0f),
  m_smoothedHandDeltas(0,0),
  m_ppmm(96.0f/25.4f),
  m_scrollOperation(nullptr)
{
  smoothedDeltaX.SetInitialValue(0.0f);
  smoothedDeltaY.SetInitialValue(0.0f);
  smoothedDeltaX.SetSmoothStrength(0.3f);
  smoothedDeltaY.SetSmoothStrength(0.3f);
}

StateMachine::~StateMachine(void)
{
  m_scrollOperation.reset();
}

// Transition Checking Loop
void StateMachine::AutoFilter(std::shared_ptr<Leap::Hand> pHand, const HandData& handData, const FrameTime& frameTime, const Scroll& scroll, OSCState& state) {
  std::lock_guard<std::mutex> lk(m_lock);

  if(m_state == OSCState::FINAL) {
    return;
  }
  
  m_lastScrollReleaseTimestep += frameTime.deltaTime;
  
  // Check for normal transitions depending on our current state.
  // Define possible State Transitions
  OSCState desiredState = m_state;
  switch(m_state) {
    case OSCState::BASE:
    case OSCState::EXPOSE_ACTIVATOR_FOCUSED:
    case OSCState::MEDIA_MENU_FOCUSED:
      desiredState = resolvePose(handData.handPose);
      break;
    case OSCState::EXPOSE_FOCUSED:
    case OSCState::FINAL: //Here for completeness. We should never ever hit this one.
    default:
      //Don't do anything. Transitions from these states are event driven.
      break;
  }
  
  if (m_state != desiredState) {
    m_desiredTransitions.push(desiredState);
  }
  
  smoothedDeltaX.SetGoal(handData.locationData.dX);
  smoothedDeltaY.SetGoal(handData.locationData.dY);
  
  m_smoothedHandDeltas = Vector2(smoothedDeltaX, smoothedDeltaY);

  //Fill in our AutoFilter outputs (defaults)
  state = m_state;

  if (m_scrollType == ScrollType::HAND_SCROLL) {
    doHandScroll(scroll, handData.locationData);
  }
  else if (m_scrollType == ScrollType::PINCH_SCROLL) {
    doPinchScroll(scroll, handData.locationData, handData.pinchData);
  }
  
  m_lastHandLocation = handData.locationData.screenPosition();
}

//returns 'to' if a valid transition, or the alternative state if not.
OSCState StateMachine::validateTransition(OSCState to) const {
  
  if ( to == OSCState::SCROLLING && m_state != OSCState::BASE ) {
    return m_state;
  }
  else if (to == OSCState::MEDIA_MENU_FOCUSED || to == OSCState::EXPOSE_ACTIVATOR_FOCUSED ) {
    if ( m_state == OSCState::SCROLLING || m_lastScrollReleaseTimestep <= 1000000) {
      return m_state;
    }
  }
  else if (to == OSCState::MEDIA_MENU_FOCUSED ) {
    if ( m_smoothedHandDeltas.norm() > transitionConfigs::MAX_HAND_DELTA_FOR_POSE_TRANSITION ) {
      return m_state;
    }
  }
  
  return to;
}

void StateMachine::performNextTransition() {
  if ( m_desiredTransitions.size() <= 0 ) {
    return;
  }
  
  OSCState desiredState = m_desiredTransitions.front();
  m_desiredTransitions.pop();
  
  desiredState = validateTransition(desiredState);
  
  if (m_state == desiredState) {
    return;
  }
  
  if (desiredState == OSCState::FINAL) {
    if ( m_state == OSCState::EXPOSE_FOCUSED ) {
      m_evp->Shutdown();
    }
    else if ( m_state == OSCState::SCROLLING ) {
      m_scrollOperation.reset();
    }
  }
  else if (desiredState == OSCState::EXPOSE_FOCUSED){
    if ( m_state == OSCState::SCROLLING ) {
      m_scrollOperation.reset();
    }
  }
  else if (desiredState == OSCState::SCROLLING) {
    bool didStartScroll = initializeScroll(m_lastHandLocation);
    if ( !didStartScroll ) { return; }
  }
  
  if ( m_state == OSCState::EXPOSE_FOCUSED ) {
    m_evp->Shutdown();
  }

  m_state = desiredState;
}

OSCState StateMachine::resolvePose(HandPose pose) const {
  switch (pose) {
  case HandPose::OneFinger:
    return OSCState::MEDIA_MENU_FOCUSED;
  case HandPose::ThreeFingers:
    return OSCState::EXPOSE_ACTIVATOR_FOCUSED;
  default:
    return OSCState::BASE;
  }
}

bool StateMachine::initializeScroll(const Vector2& scrollLocation) {
  AutowiredFast<OSCursor> cursor;
  if (cursor) {
    auto screenPosition = scrollLocation;
    OSPoint point{ static_cast<float>(screenPosition.x()), static_cast<float>(screenPosition.y()) };
    
    // Move cursor
    AutowiredFast<OSCursor> cursor;
    if (cursor) {
      // Set the current cursor position
      cursor->SetCursorPos(point);
      // Make the application at the point become active
      NativeWindow::RaiseWindowAtPosition(point.x, point.y);
    }
    
    // Update the pixels-per-inch for scrolling on this screen
    float ppi = 96.0f;
    AutowiredFast<OSVirtualScreen> virtualScreen;
    if (virtualScreen) {
      OSScreen screen = virtualScreen->ClosestScreen(point);
      ppi = screen.PixelsPerInch();
    }
    m_ppmm = ppi / 25.4f;
    
    m_scrollOperation = m_windowScroller->BeginScroll();
    if (m_scrollOperation) {
      return true;
    }
  }
  return false;
}

void StateMachine::doHandScroll(const Scroll& scroll, const HandLocation& handLocation) {

  const double deltaScrollMultiplier = 0.15;
  const double deltaScrollThreshold = 0.15;
  Vector2 deltaScroll = -deltaScrollMultiplier*scroll.m_deltaScrollMM.head<2>();

  switch (m_state) {
    case OSCState::SCROLLING:
    if (deltaScroll.squaredNorm() < deltaScrollThreshold) {
      m_scrollOperation.reset();
      m_desiredTransitions.push(OSCState::BASE);
    }
    break;
  default:
    if (deltaScroll.squaredNorm() > deltaScrollThreshold && m_state == OSCState::BASE) {
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
        m_desiredTransitions.push(OSCState::SCROLLING);
      }
    }
    break;
  }

  m_handDelta = deltaScroll;
}

void StateMachine::doPinchScroll(const Scroll& scroll, const HandLocation& handLocation, const HandPinch& pinch) {
  Vector2 deltaScroll = Vector2::Zero();

  if ( m_state == OSCState::SCROLLING )
  {
    if (!pinch.isPinching) {
      m_desiredTransitions.push(OSCState::BASE);
      
      //Move to transition
      m_scrollOperation.reset();
      m_lastScrollReleaseTimestep = 0.0f;
      m_desiredTransitions.push(OSCState::BASE);
    }
    deltaScroll = Vector2{ handLocation.dmmX, handLocation.dmmY };
  }
  else {
    if (pinch.isPinching && m_state == OSCState::BASE) {
      m_desiredTransitions.push(OSCState::SCROLLING);
    }
  }

  m_handDelta = deltaScroll;
  //m_scrollState = scrollState;
}

void StateMachine::RequestTransition(OSCState requestedState) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_desiredTransitions.push(requestedState);
}

void StateMachine::OnHandVanished() {
  RequestTransition(OSCState::FINAL);
}

// Distpatch Loop
void StateMachine::Tick(std::chrono::duration<double> deltaT) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  smoothedDeltaX.Update(deltaT.count());
  smoothedDeltaY.Update(deltaT.count());
  
  //Perform any transitions waiting in the transition queue
  while ( m_desiredTransitions.size() > 0 ) {
    performNextTransition();
  }

  if ( m_state == OSCState::FINAL ) {
      // Remove our controls from the scene graph
      m_mediaViewStateMachine->RemoveFromParent();
      m_exposeActivationStateMachine->RemoveFromParent();
      m_cursorView->RemoveFromParent();

      // Shutdown the context
      m_context->SignalShutdown();
      
      // Remove our own reference to the context
      m_context.reset();
  }
  else if ( m_state == OSCState::SCROLLING)
  {
    m_scrollOperation->ScrollBy(0.0f, (float)m_handDelta.y() * SCROLL_SENSITIVITY * m_ppmm);
  }

  m_handDelta = Vector2(0, 0);
}
