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
  ASSERT_EQ(desiredPos, measuredPos) << "Cursor position was not updated as expected";

  // Also verify that the cursor went back to the place it was in before the test started
  auto finalPos = osCursor->GetCursorPos();
  ASSERT_EQ(originalPos, finalPos) << "Cursor did not return to the pre-test location";
}