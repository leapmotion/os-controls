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

class ShortcutsStateClass {
protected:
  ShortcutsState m_state;

public:
  ShortcutsStateClass() {
    m_state = ShortcutsState::BASE;
  }

  operator ShortcutsState&() {
    return m_state;
  }

  operator const ShortcutsState&() const {
    return m_state;
  }

  ShortcutsStateClass& operator = (const ShortcutsState& rhs) {
    m_state = rhs;
    return *this;
  }
};

class ShortcutsStateChangeEvent {
public:
  virtual void RequestTransition(ShortcutsState requestedState) = 0;
};

/// <summary>
/// The set of possible inputs to the Shortcuts state machine
/// </summary>
enum class ShortcutsInputs {
  NoInput,

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

class ShortcutsInputsClass {
protected:
  ShortcutsInputs m_inputs;

public:
  ShortcutsInputsClass() {
    m_inputs = ShortcutsInputs::NoInput;
  }

  operator ShortcutsInputs&() {
    return m_inputs;
  }

  operator const ShortcutsInputs&() const {
    return m_inputs;
  }

  ShortcutsInputsClass& operator = (const ShortcutsInputs& rhs) {
    m_inputs = rhs;
    return *this;
  }
};
