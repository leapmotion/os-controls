#pragma once
#include <Leap.h>

/// <summary>
/// Broadcast notification interface for Leap events
/// </summary>
class LeapInputListener {
public:
  ~LeapInputListener(void) {}
  virtual void OnLeapFrame(const Leap::Frame& frame) = 0;
};