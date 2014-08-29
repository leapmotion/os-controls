#include "stdafx.h"
#include "WindowScrollerMac.h"

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerMac;
}

void WindowScrollerMac::DoScrollBy(uint32_t virtualX, uint32_t virtualY, double unitsX, double unitsY) {

}