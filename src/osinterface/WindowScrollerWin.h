#pragma once
#include "WindowScroller.h"

class WindowScrollerWin:
  public IWindowScroller
{
protected:
  void DoScrollBy(const OSPoint& deltaPixel, const OSPoint& deltaLine, bool isMomentum) override;
};
