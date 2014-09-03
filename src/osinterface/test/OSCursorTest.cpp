#include "stdafx.h"
#include "OSCursor.h"

class OSCursorTest:
  public testing::Test
{};

TEST_F(OSCursorTest, SetGet) {
  AutoRequired<OSCursor> osCursor;
  const auto originalPos = osCursor->GetCursorPos();

  // Set the position to something known:
  const OSPoint desiredPos {100, 101};
  osCursor->SetCursorPos(desiredPos);
  const auto measuredPos = osCursor->GetCursorPos();

  // Put it back:
  osCursor->SetCursorPos(originalPos);

  // Now verify that our attempt to assign the position went as planned:
  const char* msg = "Cursor position was not updated as expected";
  ASSERT_FLOAT_EQ(desiredPos.x, measuredPos.x) << msg;
  ASSERT_FLOAT_EQ(desiredPos.y, measuredPos.y) << msg;

  // Also verify that the cursor went back to the place it was in before the test started
  auto finalPos = osCursor->GetCursorPos();
  const char* msg2 = "Cursor did not return to the pre-test location";
  ASSERT_FLOAT_EQ(originalPos.x, finalPos.x) << msg2;
  ASSERT_FLOAT_EQ(originalPos.y, finalPos.y) << msg2;
}