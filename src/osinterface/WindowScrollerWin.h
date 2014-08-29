#pragma once
#include "WindowScroller.h"

class WindowScrollerWin:
  public IWindowScroller
{
protected:
  void DoScrollBy(float deltaX, float deltaY, bool isMomentum) override;
};
