#pragma once
#include "StateMachineContext.h"
#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "uievents/OSCDomain.h"
#include "uievents/Updatable.h"

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
  public Updatable
{
public:
  StateMachine(void);
  ~StateMachine(void);

  void StateMachine::AutoFilter(Leap::Hand* pHand, const HandLocation& handLocation, const HandPose handPose, OSCState& state);

  // Updatable overrides:
  void Update(std::chrono::duration<double> deltaT) override;

private:
  // Our current state
  OSCState m_state;

  // Lets us store a pointer to our current context so we can keep it around.  This gives
  // us the ability to decide when we want to be evicted by just resetting this value.
  AutoCurrentContext m_context;
};

