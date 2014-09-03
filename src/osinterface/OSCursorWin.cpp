#include "stdafx.h"
#include "OSCursorWin.h"

OSCursor* OSCursor::New(void) {
  return new OSCursorWin;
}

OSPoint OSCursorWin::GetCursorPos(void) const {
  POINT retVal;
  ::GetCursorPos(&retVal);
  return OSPoint{retVal.x, retVal.y};
}

void OSCursorWin::SetCursorPos(OSPoint point) const {
  ::SetCursorPos(point.x, point.y);
}