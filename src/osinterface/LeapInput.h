#pragma once
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
  Leap::Listener
{
public:
  LeapInput(void);
  ~LeapInput(void);

private:
  AutoRequired<Leap::Controller> m_controller;
  Autowired<OSVirtualScreen> m_virtualScreen;
  bool m_isAcceptingInput;

  // Event raised when a leap frame comes in
  AutoFired<LeapInputListener> m_listener;

  // Determine whether or not to accepting Leap input
  bool AcceptInput(void) const;

  // Leap::Listener overrides:
  void onDisconnect(const Leap::Controller& controller) override;
  void onFrame(const Leap::Controller& controller) override;
};
