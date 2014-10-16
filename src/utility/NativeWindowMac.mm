#include "stdafx.h"
#include "NativeWindow.h"

#include <AppKit/NSWindow.h>

void NativeWindow::RaiseWindowAtPosition(float x, float y) {
  NSPoint point{x, y};
  @autoreleasepool {
    const CGWindowID windowID = (CGWindowID) [NSWindow windowNumberAtPoint:point belowWindowWithWindowNumber:0];
    if (!windowID) {
      return;
    }
    CFArrayRef windowInfo = CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, windowID);
    bool isValid = [(__bridge id)windowInfo count] > 0;
    CFRelease(windowInfo);
    if (!isValid) {
      return;
    }
    // FIXME
  }
}
