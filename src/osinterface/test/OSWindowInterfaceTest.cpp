#include "stdafx.h"
#include "WindowCreatingTestFixture.h"
#include "OSWindowManagerInterface.h"

class OSWindowInterfaceTest:
  public testing::Test
{};

TEST_F(OSWindowInterfaceTest, CanEnumerateTopLevelWindows) {
  AutoRequired<WindowCreatingTestFixture> wctf;
  auto props = wctf->CreateTestWindow();

  // Now see if we can enumerate to this window as one of the top-level windows:
  AutoRequired<OSWindowManagerInterface> oswmi;
  auto windows = oswmi->EnumerateTopLevel();

  // At the minimum, we should find one top-level window
  ASSERT_FALSE(windows.empty()) << "Child enumeration failed to return a single window";
}