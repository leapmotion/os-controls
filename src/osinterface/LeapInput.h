#pragma once
#include <autowiring/ContextMember.h>
#include <Leap.h>

class FrameFragmenter;

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

  // We're going to need this because we pass control directly to the frame fragmenter
  Autowired<FrameFragmenter> m_fragmenter;

  void onFrame(const Leap::Controller& controller) override;
};
