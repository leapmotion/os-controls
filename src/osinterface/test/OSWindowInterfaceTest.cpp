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
};

TEST_F(OSWindowInterfaceTest, SimpleTopLevelEnumerateo) {
  AutoRequired<WindowCreatingTestFixture> wctf;
  AutoRequired<OSWindowMonitor> oswmi;
  const auto testWindowProps = wctf->CreateTestWindow();
  bool found = false;
  auto finderLambda = [&found, &testWindowProps](OSWindow& os) {
    found |= os.GetOwnerPid() == testWindowProps.pid;
  };

  // Verify that we can't find the window at this point
  oswmi->Enumerate(finderLambda);
  ASSERT_FALSE(found) << "Enumerated a window before it should have been discoverable";

  // Try a few times to find the window with increasing delays:
  for(auto& delayTime : sc_delayTimes) {
    oswmi->Enumerate(finderLambda);
    if(found)
      break;

    // Tick and cycle:
    oswmi->Tick(std::chrono::duration<double>(0.0));
    std::this_thread::sleep_for(delayTime);
  }
  ASSERT_TRUE(found) << "Failed to find a created window";
}
