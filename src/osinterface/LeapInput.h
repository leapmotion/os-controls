#pragma once
#include <autowiring/ContextMember.h>
#include <Leap.h>

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
  AutoRequired<AutoPacketFactory> m_factory;

  void onFrame(const Leap::Controller& controller) override;
};
