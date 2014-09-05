// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSWindowMac.h"
#include <Primitives.h>

#include <AppKit/NSWindow.h>

OSWindowMac::OSWindowMac(CGWindowID windowID) :
  windowID(windowID)
{
  @autoreleasepool {
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly |
                                                       kCGWindowListExcludeDesktopElements |
                                                       kCGWindowListOptionIncludingWindow |
                                                       kCGWindowListOptionOnScreenBelowWindow, windowID);
    NSArray* windowArray = CFBridgingRelease(windowList);
    for (NSDictionary* entry in windowArray) {
      if ([[entry objectForKey:(id)kCGWindowNumber] unsignedIntValue] == windowID) {
        NSString *applicationName = [entry objectForKey:(id)kCGWindowOwnerName];
        const pid_t pid = static_cast<pid_t>([[entry objectForKey:(id)kCGWindowOwnerPID] intValue]);
        NSDictionary* windowBounds = [entry objectForKey:(id)kCGWindowBounds];
        CGRect bounds = NSZeroRect;
        CGRectMakeWithDictionaryRepresentation(reinterpret_cast<CFDictionaryRef>(windowBounds), &bounds);
        break;
      }
    }
  }
}

OSWindowMac::~OSWindowMac(void)
{
}

bool OSWindowMac::IsValid(void) {
  return true;
}

std::shared_ptr<OSApp> OSWindowMac::GetOwnerApp(void) {
  return nullptr;
}

uint32_t OSWindowMac::GetOwnerPid(void) {
  return 0;
}

bool OSWindowMac::GetFocus(void) {
  return false;
}

void OSWindowMac::SetFocus(void) {
}

std::wstring OSWindowMac::GetTitle(void) {
  std::wstring retVal;
  return retVal;
}

OSPoint OSWindowMac::GetPosition(void) {
  OSPoint retVal;
  retVal.x = 0;
  retVal.y = 0;
  return retVal;
}

OSSize OSWindowMac::GetSize(void) {
  OSSize retVal;
  retVal.width = 0;
  retVal.height = 0;
  return retVal;
}

void OSWindowMac::Cloak(void) {

}

void OSWindowMac::Uncloak(void) {
}

bool OSWindowMac::IsVisible(void) const {
  return true;
}
