#pragma once

/// <summary>
/// The set of possible states that the Shortcuts state machine can reside in
/// </summary>
enum class ShortcutsState {
  // The ground state, no controls visible, do nothing special
  BASE,

  //The state where the media Raidal Menu menu has focus
  MEDIA_MENU_FOCUSED,

  //The state where the desktop switcher / expose accessor radial menu has focus
  EXPOSE_ACTIVATOR_FOCUSED,
  
  //The state where the expose full screen interaction view is visible
  EXPOSE_FOCUSED,
  
  SCROLLING,
  // The final state, transitioning here indicates that the AutoFilter routine has received a
  // null Leap::Hand and that this control will not receive further inputs.
  FINAL
};

class ShortcutsStateChangeEvent{
public:
  virtual void RequestTransition(ShortcutsState requestedState) = 0;
};

/// <summary>
/// The set of possible inputs to the Shortcuts state machine
/// </summary>
enum class ShortcutsInputs {
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
