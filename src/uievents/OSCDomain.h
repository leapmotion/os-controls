#pragma once

/// <summary>
/// The set of possible states that the OS controls state machine can reside in
/// </summary>
enum class OSCState {
  // The ground state, no controls visible, do nothing special
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