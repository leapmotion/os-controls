#pragma once
#include "WindowCreatingTestFixture.h"

class WindowCreatingTestFixtureMac:
  public WindowCreatingTestFixture
{
public:
  ~WindowCreatingTestFixtureMac(void);

  // WindowCreatingTestFixture overrides:
  TestWindowProperties CreateTestWindow(void) override;

private:
  std::vector<pid_t> m_createdProcesses;
};
