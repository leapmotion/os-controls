#pragma once
#include "WindowCreatingTestFixture.h"

class WindowCreatingTestFixtureMac:
  public WindowCreatingTestFixture
{
public:
  ~WindowCreatingTestFixtureMac(void);

  // WindowCreatingTestFixture overrides:
  const wchar_t* GetApplicationPath(void) const { return L"/Applications/TextEdit.app"; }
  TestWindowProperties CreateTestWindow(void) override;

private:
  std::vector<pid_t> m_createdProcesses;
};
