#pragma once
#include <autowiring/CoreThread.h>
#include "osinterface/LeapInputListener.h"

class ErrorDialogs :
  public CoreThread,
  public LeapInputListener
{
public:
  ErrorDialogs();

private:
  void RaiseErrorMessages();

  void OnLeapFrame(const Leap::Frame& frame) override;
};

