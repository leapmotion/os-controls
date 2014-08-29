#pragma once
#include "WindowScroller.h"

class WindowScrollerMac:
  public IWindowScroller
{
protected:
  void DoScrollBy(uint32_t virtualX, uint32_t virtualY, double unitsX, double unitsY) override;
};