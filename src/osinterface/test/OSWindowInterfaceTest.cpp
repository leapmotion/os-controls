#include "stdafx.h"
#include "WindowCreatingTestFixture.h"

class OSWindowInterfaceTest:
  public testing::Test
{};

TEST_F(OSWindowInterfaceTest, CanEnumerateTopLevelWindows) {
#if 0
  AutoRequired<WindowCreatingTestFixture> wctf;
  const auto testWindowProps = wctf->CreateTestWindow();

  // Now see if we can enumerate to this window as one of the top-level windows:
  AutoRequired<OSWindowManagerInterface> oswmi;
  auto windows = oswmi->EnumerateTopLevel();

  // At the minimum, we should find one top-level window
  ASSERT_FALSE(windows.empty()) << "Child enumeration failed to return a single window";

  // Now try to find our window:
  auto q = std::find_if(
      windows.begin(),
      windows.end(),
      [&testWindowProps] (const std::shared_ptr<OSWindowNode>& node) {
        return node->GetOwnerPid() == testWindowProps.pid;
      }
    );
  ASSERT_NE(windows.end(), q) << "Failed to find the test window in the enumeration of top-level windows";

  // Validate names are what we expect to find:
  std::shared_ptr<OSWindowNode> foundTestWindow = *q;
#endif
}
