#pragma once
#include "StateMachineContext.h"
#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "cursor/CursorView.h"
#include "expose/ExposeActivationStateMachine.h"
#include "mediaview/MediaViewController.h"
#include "mediaview/MediaViewStateMachine.h"
#include "interaction/HandEventListener.h"
#include "interaction/HandActivationRecognizer.h"
#include "interaction/HandActivationRecognizer.h"
#include "interaction/ScrollRecognizer.h"
#include "interaction/HandDataCombiner.h"
#include "osinterface/WindowScroller.h"
#include "uievents/ShortcutsDomain.h"
#include "uievents/Updatable.h"
#include "Animation.h"

#include <queue>

#define USE_HAND_SCROLL 1

namespace Leap {
  class Hand;
}

class ExposeViewStateMachine;
class Config;

/// <summary>
/// The central state machine concept
/// </summary>
/// <remarks>
/// This state machine has global knowledge of all interior components of the Shortcuts
/// interaction system.  It is a top-level system, and 
/// </remarks>

class StateMachine:
  public ContextMember,
  public Updatable,
  public HandEventListener,
  public ShortcutsStateChangeEvent
{
public:
  StateMachine(void);
  ~StateMachine(void);
  
  void AutoFilter(std::shared_ptr<Leap::Hand> pHand, const HandData& handData, const FrameTime& frameTime, const Scroll& scroll, ShortcutsState& state);
  
  void OnHandVanished() override;
  void RequestTransition(ShortcutsState requestedState) override;

  // Updatable overrides:
  void Tick(std::chrono::duration<double> deltaT) override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  ShortcutsState validateTransition(ShortcutsState to) const;
  void performNextTransition();
  
  bool pointIsOnScreen(const Vector2& point) const;

  ShortcutsState resolvePose(HandPose pose) const;
  
  bool initializeScroll(const Vector2& scrollPosition); // returns if the initialization was sucessful
  void doHandScroll(const Scroll& scroll, const HandLocation& handLocation);
  void doPinchScroll(const Scroll& scroll, const HandLocation& handLocation, const HandPinch& pinch);

  std::mutex m_lock;

  ShortcutsState m_state;
  std::queue<ShortcutsState> m_desiredTransitions;
  
  enum class ScrollType {
    HAND_SCROLL,
    PINCH_SCROLL
  };
  ScrollType m_scrollType;

  Vector2 m_handDelta; //in millimeters
  Vector2 m_lastHandLocation; //in screen coordinates
  const float SCROLL_SENSITIVITY = 1.3f * 96.0f / 25.4f;
  float m_lastScrollReleaseTimestep;
  
  Smoothed<float> smoothedDeltaX;
  Smoothed<float> smoothedDeltaY;
  Vector2 m_smoothedHandDeltas;

  // Pixels-per-millimeter for use with the current scroll operation
  float m_ppmm;
  
  std::shared_ptr<IScrollOperation> m_scrollOperation;

  AutoRequired<CursorView> m_cursorView;
  AutoRequired<MediaViewController> m_mediaViewController;
  Autowired<IWindowScroller> m_windowScroller;
  Autowired<sf::RenderWindow> m_renderWindow;
  Autowired<Config> m_config;

  AutoRequired<MediaViewStateMachine> m_mediaViewStateMachine;
  AutoRequired<ExposeActivationStateMachine> m_exposeActivationStateMachine;
  AutoRequired<ExposeViewStateMachine> m_evp;
  
  // Lets us store a pointer to our current context so we can keep it around.  This gives
  // us the ability to decide when we want to be evicted by just resetting this value.
  AutoCurrentContext m_context;
};
