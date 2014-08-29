#include "stdafx.h"
#include "WindowScrollerWin.h"

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerWin;
}

void WindowScrollerWin::DoScrollBy(float deltaX, float deltaY, bool isMomentum) {

}
