#pragma once
#include <autowiring/CoreThread.h>
#include "osinterface/LeapInputListener.h"

class ErrorDialogs :
  public CoreThread
{
public:
  ErrorDialogs();

private:
  void RaiseErrorMessages();
};
