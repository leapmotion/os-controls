#include "stdafx.h"
#include "WindowCreatingTestFixture.h"
#include "OSApp.h"
#include "OSWindow.h"
#include "OSWindowMonitor.h"
#include "utility/NativeWindow.h"
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
      oswmi->Tick(std::chrono::seconds(1));
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

  // Make sure that all of these windows are visible--they shouldn't show up if they are invisible
  for(auto wnd : wnds)
    ASSERT_TRUE(wnd->IsVisible()) << "Enumerator returned an invisible window";
}

TEST_F(OSWindowInterfaceTest, ValidateWindowSetFocus) {
  // Create a window and verify we can find it:
  const auto testWindowProps = wctf->CreateTestWindow();

  AutoRequired<OSWindowMonitor> oswmi;
  auto wnds = RetryFindWindow(testWindowProps.pid, sc_delayTimes);
  ASSERT_FALSE(wnds.empty()) << "Failed to enumerate a created top-level window";

  // Bring this window to the foreground so that we can be assured it will be under the point we're
  // about to check
  auto osw = wnds[0];
  osw->SetFocus();

  // Scan again to see what happened when we set the window focus:
  oswmi->Tick(std::chrono::seconds(1));

  // This window should be frontmost because we just created it
  int zorder = osw->GetZOrder();
  ASSERT_GE(0, zorder) << "A window's z-order was not properly calculated.";
  ASSERT_EQ(0, zorder) << "Window which should have been the frontmost application was not reported as the frontmost";
}

TEST_F(OSWindowInterfaceTest, WindowRaiser) {
  // Create the test window:
  const auto testWindowProps = wctf->CreateTestWindow();

  AutoRequired<OSWindowMonitor> oswmi;
  auto wnds = RetryFindWindow(testWindowProps.pid, sc_delayTimes);
  ASSERT_FALSE(wnds.empty());
  auto osw = wnds[0];

  // Use a point hit to bring the window to the foreground:
  auto pos = osw->GetPosition();
  NativeWindow::RaiseWindowAtPosition(pos.x, pos.y);

  // Scan again to see what happened when we set the window focus:
  oswmi->Tick(std::chrono::seconds(1));
  wnds = RetryFindWindow(testWindowProps.pid, sc_delayTimes);
  osw = wnds[0];
  int zorder = osw->GetZOrder();
  ASSERT_EQ(0, zorder) << "Raising a window from a point did not correctly make it the topmost in the z-order";
}
