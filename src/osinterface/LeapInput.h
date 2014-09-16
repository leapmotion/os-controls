#pragma once
#include <autowiring/ContextMember.h>
#include <Leap.h>

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

  // Event raised when a leap frame comes in
  AutoFired<LeapInputListener> m_listener;

  // Leap::Listener overrides:
  void onServiceConnect(const Leap::Controller& controller) override;
  void onConnect(const Leap::Controller& controller) override;
  void onFrame(const Leap::Controller& controller) override;
  void onDisconnect(const Leap::Controller& controller) override;
  void onServiceDisconnect(const Leap::Controller& controller) override;
};
