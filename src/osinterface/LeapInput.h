#pragma once
#include "PauseInteractionEvent.h"

#include <autowiring/ContextMember.h>
#include <Leap.h>
class OSVirtualScreen;
class FrameFragmenter;
class LeapInputListener;

/// <summary>
/// Packet source which interfaces with the leap API
/// </summary>
class LeapInput:
  public ContextMember,
  public PauseInteractionEvent,
  Leap::Listener
{
public:
  LeapInput(void);
  ~LeapInput(void);

  // PauseInteractionEvent overrides:
  void PauseChanged(bool paused) override;

private:
  AutoRequired<Leap::Controller> m_controller;
  Autowired<OSVirtualScreen> m_virtualScreen;
  bool m_isAcceptingInput;
  bool m_isPaused;

  // Event raised when a leap frame comes in
  AutoFired<LeapInputListener> m_listener;

  // Determine whether or not to accept Leap input
  bool AcceptInput(void) const;

  // Abort any interaction going on with the device
  void AbortInput(void);

  // Leap::Listener overrides:
  void onServiceConnect(const Leap::Controller& controller) override;
  void onDisconnect(const Leap::Controller& controller) override;
  void onFocusLost(const Leap::Controller& controller) override;
  void onFrame(const Leap::Controller& controller) override;
};
