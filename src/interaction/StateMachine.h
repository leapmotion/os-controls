#pragma once
#include "StateMachineContext.h"
#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "uievents/OSCDomain.h"
#include "uievents/Updatable.h"
#include "graphics/CursorView.h"
#include "interaction/HandEventListener.h"
#include "interaction/MediaViewController.h"
#include "interaction/HandPinchRecognizer.h"
#include "interaction/HandPinchRecognizer.h"
#include "interaction/ScrollRecognizer.h"
#include "interaction/HandDataCombiner.h"
#include "graphics/MediaViewStateMachine.h"
#include "osinterface/WindowScroller.h"
#include "graphics/ExposeActivationStateMachine.h"

#define USE_HAND_SCROLL 1

namespace Leap {
  class Hand;
}

class ExposeViewStateMachine;

/// <summary>
/// The central state machine concept
/// </summary>
/// <remarks>
/// This state machine has global knowledge of all interior components of the OS controls
/// interaction system.  It is a top-level system, and 
/// </remarks>
class StateMachine:
  public ContextMember,
  public Updatable,
  public HandEventListener,
  public ExposeActivatorEventListener
{
public:
  StateMachine(void);
  ~StateMachine(void);
  
  void AutoFilter(std::shared_ptr<Leap::Hand> pHand, const HandData& handData, const FrameTime& frameTime, const Scroll& scroll, OSCState& state, ScrollState& scrollState);
  
  void OnHandVanished();
  void OnActivateExpose();

  // Updatable overrides:
  void Tick(std::chrono::duration<double> deltaT) override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  std::mutex m_lock;

  // Our current state
  OSCState m_state;
  
  ScrollState m_scrollState;
  Vector2 m_handDelta; //in millimeters
  const float SCROLL_SENSITIVITY = 1.3f * 96.0f / 25.4f;
  float m_lastScrollReleaseTimestep;

  // Pixels-per-millimeter for use with the current scroll operation
  float m_ppmm;
  
  std::shared_ptr<IScrollOperation> m_scrollOperation;

  AutoRequired<CursorView> m_cursorView;
  AutoRequired<MediaViewStateMachine> m_mediaViewStateMachine;
  AutoRequired<ExposeActivationStateMachine> m_exposeActivationStateMachine;
  AutoRequired<MediaViewController> m_mediaViewController;
  AutoRequired<ExposeViewStateMachine> m_evp;
  Autowired<IWindowScroller> m_windowScroller;
  
  // Lets us store a pointer to our current context so we can keep it around.  This gives
  // us the ability to decide when we want to be evicted by just resetting this value.
  AutoCurrentContext m_context;
};
