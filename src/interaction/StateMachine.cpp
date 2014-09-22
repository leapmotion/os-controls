#include "stdafx.h"
#include "StateMachine.h"
#include "InteractionConfigs.h"
#include "interaction/HandDataCombiner.h"
#include "uievents/ShortcutsDomain.h"

#include "osinterface/OSCursor.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/OSWindow.h"
#include "utility/NativeWindow.h"
#include "utility/Config.h"

#include "Color.h"

StateMachine::StateMachine(void) :
  ContextMember("StateMachine"),
  m_state(ShortcutsState::BASE),
  m_scrollType(ScrollType::PINCH_SCROLL),
  m_handDelta(0.0,0.0),
  m_lastHandLocation(0.0f,0.0f),
  m_lastScrollReleaseTimestep(0.0f),
  m_smoothedHandDeltas(0,0),
  m_ppmm(96.0f/25.4f),
  m_scrollOperation(nullptr)
{
  // Smoothed pixel deltas for movement
  smoothedDeltaX.SetInitialValue(0.0f);
  smoothedDeltaY.SetInitialValue(0.0f);
  smoothedDeltaX.SetSmoothStrength(0.3f);
  smoothedDeltaY.SetSmoothStrength(0.3f);
  
  // Smoothed mm deltas for scrolling
  m_handDeltaMM_X.SetInitialValue(0.0f);
  m_handDeltaMM_X.SetSmoothStrength(0.3f);
  m_handDeltaMM_Y.SetInitialValue(0.0f);
  m_handDeltaMM_Y.SetSmoothStrength(0.3f);
}

StateMachine::~StateMachine(void)
{
  m_scrollOperation.reset();
}

// Transition Checking Loop
void StateMachine::AutoFilter(const HandData& handData, const FrameTime& frameTime, ShortcutsState& state) {
  std::lock_guard<std::mutex> lk(m_lock);

  if(m_state == ShortcutsState::FINAL) {
    return;
  }
  
  m_lastScrollReleaseTimestep += frameTime.deltaTime;
  
  // Check for normal transitions depending on our current state.
  // Define possible State Transitions
  ShortcutsState desiredState = m_state;
  switch(m_state) {
    case ShortcutsState::BASE:
    case ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED:
    case ShortcutsState::MEDIA_MENU_FOCUSED:
      desiredState = resolvePose(handData.handPose);
      break;
    case ShortcutsState::EXPOSE_FOCUSED:
    case ShortcutsState::FINAL: //Here for completeness. We should never ever hit this one.
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
    doHandScroll(handData.scroll, handData.locationData);
  }
  else if (m_scrollType == ScrollType::PINCH_SCROLL) {
    doPinchScroll(handData.scroll, handData.locationData, handData.pinchData);
  }
  
  m_lastHandLocation = handData.locationData.screenPosition();
}

//returns 'to' if a valid transition, or the alternative state if not.
ShortcutsState StateMachine::validateTransition(ShortcutsState to) const {
  const bool enableWS = m_config->Get<bool>("enableWindowSelection");
  const bool enableScroll = m_config->Get<bool>("enableScroll");
  const bool enableMedia = m_config->Get<bool>("enableMedia");
  
  if (!enableMedia && to == ShortcutsState::MEDIA_MENU_FOCUSED)
    return m_state;

  if (!enableWS && to == ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED)
    return m_state;

  if (!enableScroll && to == ShortcutsState::SCROLLING)
    return m_state;

  if ( to == ShortcutsState::SCROLLING && (m_state != ShortcutsState::BASE || !pointIsOnScreen(m_lastHandLocation)) ) {
    return m_state;
  }
  else if (to == ShortcutsState::MEDIA_MENU_FOCUSED || to == ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED ) {
    if (to == ShortcutsState::MEDIA_MENU_FOCUSED && m_smoothedHandDeltas.norm() > transitionConfigs::MAX_HAND_DELTA_FOR_POSE_TRANSITION ) {
      return m_state;
    }
    
    if ( m_state == ShortcutsState::SCROLLING || m_lastScrollReleaseTimestep <= 1000000 || !pointIsOnScreen(m_lastHandLocation)) {
      return m_state;
    }
  }
  
  return to;
}

void StateMachine::performNextTransition() {
  if ( m_desiredTransitions.size() <= 0 ) {
    return;
  }
  
  ShortcutsState desiredState = m_desiredTransitions.front();
  m_desiredTransitions.pop();
  
  desiredState = validateTransition(desiredState);
  
  if (m_state == desiredState) {
    return;
  }
  
  if (desiredState == ShortcutsState::FINAL) {
    if (m_state == ShortcutsState::SCROLLING ) {
      m_scrollOperation.reset();
    }
  }
  else if (desiredState == ShortcutsState::EXPOSE_FOCUSED){
    if ( m_state == ShortcutsState::SCROLLING ) {
      m_scrollOperation.reset();
    }
  }
  else if (desiredState == ShortcutsState::SCROLLING) {
    bool didStartScroll = false;
    if (m_cursorView) {
      didStartScroll = initializeScroll(m_cursorView->GetCalculatedLocation());
    }
    if ( !didStartScroll ) { return; }
  }

  m_state = desiredState;
}

bool StateMachine::pointIsOnScreen(const Vector2 &point) const {
  if ( !m_renderWindow ) { return false; }
  
  bool retVal = false;
  Vector2 renderWindowSize(m_renderWindow->getSize().x, m_renderWindow->getSize().y);
  if (point.x() >= 0 &&
      point.x() <= renderWindowSize.x() &&
      point.y() >= 0 &&
      point.y() <= renderWindowSize.y()) {
    retVal = true;
  }
  
  return retVal;
}

ShortcutsState StateMachine::resolvePose(HandPose pose) const {
  switch (pose) {
  case HandPose::OneFinger:
    return ShortcutsState::MEDIA_MENU_FOCUSED;
  case HandPose::UpsideDown:
    return ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED;
  default:
    return ShortcutsState::BASE;
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
    case ShortcutsState::SCROLLING:
    if (deltaScroll.squaredNorm() < deltaScrollThreshold) {
      m_scrollOperation.reset();
      m_desiredTransitions.push(ShortcutsState::BASE);
    }
    break;
  default:
    if (deltaScroll.squaredNorm() > deltaScrollThreshold && m_state == ShortcutsState::BASE) {
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
        m_desiredTransitions.push(ShortcutsState::SCROLLING);
      }
    }
    break;
  }

  m_handDelta = deltaScroll;
}

void StateMachine::doPinchScroll(const Scroll& scroll, const HandLocation& handLocation, const HandPinch& pinch) {
  Vector2 deltaScroll = Vector2::Zero();

  if ( m_state == ShortcutsState::SCROLLING )
  {
    if (!pinch.isPinching) {
      m_desiredTransitions.push(ShortcutsState::BASE);
      
      //Move to transition
      m_scrollOperation.reset();
      m_lastScrollReleaseTimestep = 0.0f;
      m_desiredTransitions.push(ShortcutsState::BASE);
    }
    deltaScroll = Vector2{ handLocation.dmmX, handLocation.dmmY };
  }
  else {
    if (pinch.isPinching && m_state == ShortcutsState::BASE) {
      m_desiredTransitions.push(ShortcutsState::SCROLLING);
    }
  }

  m_handDelta = deltaScroll;
  //m_scrollState = scrollState;
}

void StateMachine::RequestTransition(ShortcutsState requestedState) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_desiredTransitions.push(requestedState);
}

void StateMachine::OnHandVanished() {
  RequestTransition(ShortcutsState::FINAL);
}

// Distpatch Loop
void StateMachine::Tick(std::chrono::duration<double> deltaT) {
  std::lock_guard<std::mutex> lk(m_lock);
  
  float scrollSmoothing = (fabs(m_handDelta.y()) - scrollConfigs::MM_DELTA_FOR_MAX_SMOOTHING) / (scrollConfigs::MM_DELTA_FOR_MIN_SMOOTHING - scrollConfigs::MM_DELTA_FOR_MAX_SMOOTHING);
  scrollSmoothing = std::min(1.0f, std::max(0.0f, scrollSmoothing));
  scrollSmoothing = 1 - scrollSmoothing;
  scrollSmoothing *= scrollConfigs::MAX_SCROLL_SMOOTHING;
  
  std::cout << scrollSmoothing << std::endl;
  
  m_handDeltaMM_Y.SetSmoothStrength(scrollSmoothing);
  
  scrollSmoothing = (fabs(m_handDelta.x()) - scrollConfigs::MM_DELTA_FOR_MAX_SMOOTHING) / (scrollConfigs::MM_DELTA_FOR_MIN_SMOOTHING - scrollConfigs::MM_DELTA_FOR_MAX_SMOOTHING);
  scrollSmoothing = std::min(1.0f, std::max(0.0f, scrollSmoothing));
  scrollSmoothing = 1 - scrollSmoothing;
  scrollSmoothing *= scrollConfigs::MAX_SCROLL_SMOOTHING;
  
  m_handDeltaMM_X.SetSmoothStrength(scrollSmoothing);
  
  m_handDeltaMM_X.SetGoal(m_handDelta.x());
  m_handDeltaMM_Y.SetGoal(m_handDelta.y());
  
  smoothedDeltaX.Update(static_cast<float>(deltaT.count()));
  smoothedDeltaY.Update(static_cast<float>(deltaT.count()));
  
  m_handDeltaMM_X.Update(static_cast<float>(deltaT.count()));
  m_handDeltaMM_Y.Update(static_cast<float>(deltaT.count()));
  
  //Perform any transitions waiting in the transition queue
  while ( m_desiredTransitions.size() > 0 ) {
    performNextTransition();
  }

  if ( m_state == ShortcutsState::FINAL ) {
    // Shutdown the context
    m_context->SignalShutdown();
    
    // Remove our own reference to the context
    m_context.reset();
  }
  else if ( m_state == ShortcutsState::SCROLLING)
  {
    m_scrollOperation->ScrollBy(0.0f, m_handDeltaMM_Y.Value() * SCROLL_SENSITIVITY * m_ppmm);
  }

  m_handDelta = Vector2(0, 0);
}
