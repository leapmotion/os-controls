#pragma once
#include <autowiring/CoreThread.h>

class ErrorDialogs :
  public CoreThread
{
public:
  ErrorDialogs();

  void RaiseErrorMessages();
};

