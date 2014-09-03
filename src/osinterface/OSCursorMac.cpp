#include "stdafx.h"
#include "OSCursorMac.h"

OSCursor* OSCursor::New(void) {
  return new OSCursorMac;
}

OSPoint OSCursorMac::GetCursorPos(void) const {
  CGEventRef event = CGEventCreate(0);
  OSPoint retVal = CGEventGetLocation(event);
  CFRelease(event);
  return retVal;
}

void OSCursorMac::SetCursorPos(OSPoint point) const {
  CGEventRef eventRef = CGEventCreateMouseEvent(0, kCGEventMouseMoved, point, kCGMouseButtonLeft);
  CGEventPost(kCGHIDEventTap, eventRef);
  CFRelease(eventRef);
}
