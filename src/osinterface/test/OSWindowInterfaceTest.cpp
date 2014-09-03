#include "stdafx.h"
#include "WindowCreatingTestFixture.h"
#include "OSApp.h"
#include "OSWindow.h"
#include "OSWindowMonitor.h"
#include <thread>

class OSWindowInterfaceTest:
  public testing::Test
{
public:
  OSWindowInterfaceTest(void) {
    AutoCurrentContext()->Initiate();
  }

  AutoRequired<WindowCreatingTestFixture> wctf;
  AutoRequired<OSWindowMonitor> oswmi;

  /// <summary>
  /// Lists all top-level windows owned by the specified PID a few times over a 5-second interval
  /// </summary>
  /// <remarks>
  /// This method will return as soon as any windows are found which are owned by the specified
  /// process.
  /// </remarks>
  std::vector<OSWindow*> RetryFindWindow(uint32_t pid, const std::chrono::milliseconds* pMS) {
    std::vector<OSWindow*> retVal;
    auto finderLambda = [&retVal, pid](OSWindow& os) {
      if(os.GetOwnerPid() == pid)
        retVal.push_back(&os);
    };

    // Try a few times to find the window with increasing delays:
    for(const auto* q = pMS; pMS && q->count(); q++) {
      oswmi->Enumerate(finderLambda);
      if(!retVal.empty())
        return retVal;

      // Tick and cycle:
      oswmi->Tick(std::chrono::duration<double>(0.0));
      std::this_thread::sleep_for(*q);
    }
    return retVal;
  }
};

TEST_F(OSWindowInterfaceTest, SimpleTopLevelEnumerator) {
  const auto testWindowProps = wctf->CreateTestWindow();

  // Verify that we can't find the window at this point
  auto wnds = RetryFindWindow(testWindowProps.pid, nullptr);
  ASSERT_TRUE(wnds.empty()) << "Enumerated a window before it should have been discoverable";

  wnds = RetryFindWindow(testWindowProps.pid, sc_delayTimes);
  ASSERT_FALSE(wnds.empty()) << "Failed to find a created window";
}

TEST_F(OSWindowInterfaceTest, WindowFromPoint) {
  // Create a window and verify we can find it:
  const auto testWindowProps = wctf->CreateTestWindow();

  AutoRequired<OSWindowMonitor> oswmi;
  auto wnds = RetryFindWindow(testWindowProps.pid, sc_delayTimes);
  ASSERT_FALSE(wnds.empty()) << "Failed to enumerate a created top-level window";

  // Bring this window to the foreground so that we can be assured it will be under the point we're
  // about to check
  auto osw = wnds[0];
  osw->SetFocus();

  // Get the coordinates of this window, see if we can window-from-point the same thing:
  auto pt = osw->GetPosition();

  // Adjust by one pixel right and down, so we're in the expected region of the window proper
  pt.x += 1.0f;
  pt.y += 1.0f;
  auto foundWindow = oswmi->WindowFromPoint(pt);
  ASSERT_TRUE(foundWindow != nullptr) << "Failed to find a top-level window at the expected offset";
  ASSERT_EQ(osw, foundWindow) << "Window-from-point reported an unexpected window at the specified point";
}