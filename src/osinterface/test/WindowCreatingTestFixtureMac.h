#pragma once
#include "WindowCreatingTestFixture.h"

class WindowCreatingTestFixtureMac:
  public WindowCreatingTestFixture
{
public:
    // WindowCreatingTestFixture overrides:
  TestWindowProperties CreateTestWindow(void) override;
};