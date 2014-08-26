#pragma once
#include "StateMachineContext.h"
#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "uievents/OSCDomain.h"
#include "uievents/Updatable.h"
#include "graphics/MediaView.h"
#include "graphics/CursorView.h"
#include "interaction/HandEventListener.h"

namespace Leap {
  class Hand;
}

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
  public HandEventListener
{
public:
  StateMachine(void);
  ~StateMachine(void);
  
  void AutoFilter(std::shared_ptr<Leap::Hand> pHand, const HandPose handPose, OSCState& state);
  
  void OnHandVanished();

  // Updatable overrides:
  void Tick(std::chrono::duration<double> deltaT) override;

private:
  // Our current state
  OSCState m_state;
  
  AutoConstruct<MediaView> m_mediaView;
  AutoConstruct<CursorView> m_cursorView;

  Autowired<RenderEngineNode> m_rootNode;
  
  // Lets us store a pointer to our current context so we can keep it around.  This gives
  // us the ability to decide when we want to be evicted by just resetting this value.
  AutoCurrentContext m_context;
};

