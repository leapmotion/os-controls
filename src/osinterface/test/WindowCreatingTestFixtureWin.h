#pragma once
#include "WindowCreatingTestFixture.h"

class WindowCreatingTestFixtureWin:
  public WindowCreatingTestFixture
{
public:
  ~WindowCreatingTestFixtureWin(void);

  // WindowCreatingTestFixture overrides:
  TestWindowProperties CreateTestWindow(void) override;

private:
  std::vector<HANDLE> createdHandles;
};
