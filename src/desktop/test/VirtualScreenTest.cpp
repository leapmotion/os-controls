#include "stdafx.h"
#include "VirtualScreen.h"

class VirtualScreenTest:
  public testing::Test
{};

TEST_F(VirtualScreenTest, VerifyPrimaryScreen) {
  AutoDesired<leap::VirtualScreen> vs;
  ASSERT_NE(nullptr, vs.get());

  auto primaryScreen = vs->PrimaryScreen();

  ASSERT_LT(0, primaryScreen.Width());
  ASSERT_LT(0, primaryScreen.Height());
}
