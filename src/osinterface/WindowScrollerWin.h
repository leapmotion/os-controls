#pragma once
#include "WindowScroller.h"

class WindowScrollerWin:
  public IWindowScroller
{
protected:
  void DoScrollBy(float deltaX, float deltaY, bool isMomentum) override;

private:
  // The pixels-to-line ratio
  OSPoint m_pixelsPerLine;

  // Hold on to partial lines that haven't yet been sent in an event
  OSPoint m_scrollPartialLine;
};
