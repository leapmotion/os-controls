#pragma once

/// <summary>
/// The set of possible states that the OS controls state machine can reside in
/// </summary>
enum class OSCState {
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

class OSCStateClass {
protected:
  OSCState m_state;

public:
  OSCStateClass() {
    m_state = OSCState::BASE;
  }

  operator OSCState&() {
    return m_state;
  }

  operator const OSCState&() const {
    return m_state;
  }

  OSCStateClass& operator = (const OSCState& rhs) {
    m_state = rhs;
    return *this;
  }
};

class OSCStateChangeEvent{
public:
  virtual void RequestTransition(OSCState requestedState) = 0;
};

/// <summary>
/// The set of possible inputs to the OS controls state machine
/// </summary>
enum class OSCInputs {
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

class OSCInputsClass {
protected:
  OSCInputs m_inputs;

public:
  OSCInputsClass() {
    m_inputs = OSCInputs::NoInput;
  }

  operator OSCInputs&() {
    return m_inputs;
  }

  operator const OSCInputs&() const {
    return m_inputs;
  }

  OSCInputsClass& operator = (const OSCInputs& rhs) {
    m_inputs = rhs;
    return *this;
  }
};