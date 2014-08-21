#pragma once
#include "StateMachineContext.h"
#include "graphics/Updatable.h"

namespace Leap {
  class Hand;
}

/// <summary>
/// The set of possible states that the OS controls state machine can reside in
/// </summary>
enum class OSCState {
  // The ground state
  Initial,

  // The user has made a selection, we're holding in this state to prevent other actions
  // from being engaged
  SelectionHold,

  // The final state, transitioning here indicates that the AutoFilter routine has received a
  // null Leap::Hand and that this control will not receive further inputs.
  Final
};

/// <summary>
/// The set of possible inputs to the OS controls state machine
/// </summary>
enum class OSCInputs {
  // A selection has taken place
  Selection
};

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

  void AutoFilter(Leap::Hand* pHand);

  // Updatable overrides:
  void Update(double deltaT) override;

private:
  // Our current state
  OSCState m_state;

  // Lets us store a pointer to our current context so we can keep it around.  This gives
  // us the ability to decide when we want to be evicted by just resetting this value.
  AutoCurrentContext m_context;
};

