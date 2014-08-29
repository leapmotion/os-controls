#include "stdafx.h"
#include "WindowScrollerWin.h"

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerWin;
}

void WindowScrollerWin::DoScrollBy(uint32_t virtualX, uint32_t virtualY, double unitsX, double unitsY) {

}