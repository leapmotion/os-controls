#include "stdafx.h"
#include "OSVirtualScreen.h"

class OSVirtualScreenTest:
  public testing::Test
{};

TEST_F(OSVirtualScreenTest, VerifyPrimaryScreen) {
  AutoDesired<OSVirtualScreen> vs;
  ASSERT_NE(nullptr, vs.get());

  auto primaryScreen = vs->PrimaryScreen();

  ASSERT_LT(0, primaryScreen.Width());
  ASSERT_LT(0, primaryScreen.Height());
}
