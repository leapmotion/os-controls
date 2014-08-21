#pragma once
#include "StateMachineContext.h"
#include "graphics/Updatable.h"
#include "EigenTypes.h"

namespace Leap {
  class Hand;
}

/// <summary>
/// The set of possible states that the OS controls state machine can reside in
/// </summary>
enum class OSCState {
  // The ground state
  Base,
  
  //The state where the media Raidal Menu menu has focus
  MediaMenuVisible,
  
  //The state where the desktop switcher / expose accessor radial menu has focus
  DesktopSwitcherVisible,
  
  //The state where the expose full screen interaction view is visible
  ExposeVisible,
  
  //After a selection has been made, but before a menu should be reset
  SelectionHold,

  // The final state, transitioning here indicates that the AutoFilter routine has received a
  // null Leap::Hand and that this control will not receive further inputs.
  Final
};

/// <summary>
/// The set of possible inputs to the OS controls state machine
/// </summary>
enum class OSCInputs {
  // A selection of a wedge has taken place
  Selection,
  
  //All posible finger poses
  ZeroFingers,
  OneFingers,
  TwoFingers,
  ThreeFingers,
  FourFingers,
  FiveFingers
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

  void AutoFilter(Leap::Hand* pHand, const Vector2& handCoordinates, const float handRoll, const int fingerCount);

  // Updatable overrides:
  void Update(double deltaT) override;

private:
  // Our current state
  OSCState m_state;

  // Lets us store a pointer to our current context so we can keep it around.  This gives
  // us the ability to decide when we want to be evicted by just resetting this value.
  AutoCurrentContext m_context;
};

