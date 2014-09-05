// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSWindowMac.h"
#include <Primitives.h>

#include <AppKit/NSWindow.h>
#include <Foundation/NSArray.h>

#include <cassert>

OSWindowMac::OSWindowMac(NSDictionary* info) :
  m_windowID([[info objectForKey:(id)kCGWindowNumber] unsignedIntValue]), m_info([info retain]), m_mark(0)
{
}

OSWindowMac::~OSWindowMac(void)
{
  [m_info release];
}

void OSWindowMac::UpdateInfo(NSDictionary* info) {
  assert([[info objectForKey:(id)kCGWindowNumber] unsignedIntValue] == m_windowID);
  [m_info release];
  m_info = [info retain];
}

bool OSWindowMac::IsValid(void) {
  @autoreleasepool {
    NSArray* windowArray =
        (id)CFBridgingRelease(CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, m_windowID));
    return ([windowArray count] > 0);
  }
}

std::shared_ptr<OSApp> OSWindowMac::GetOwnerApp(void) {
  return nullptr;
}

uint32_t OSWindowMac::GetOwnerPid(void) {
  return static_cast<uint32_t>([[m_info objectForKey:(id)kCGWindowOwnerPID] intValue]);
}

bool OSWindowMac::GetFocus(void) {
  return false;
}

void OSWindowMac::SetFocus(void) {
}

std::wstring OSWindowMac::GetTitle(void) {
  std::wstring retVal;
  @autoreleasepool {
    NSString *title = [m_info objectForKey:(id)kCGWindowName];
    if (title) {
      NSData* data = [title dataUsingEncoding:NSUTF32LittleEndianStringEncoding];
      retVal = std::wstring(reinterpret_cast<const wchar_t*>([data bytes]), [data length]/sizeof(wchar_t));
    }
  }
  return retVal;
}

OSPoint OSWindowMac::GetPosition(void) {
  OSPoint retVal;
  @autoreleasepool {
    NSDictionary* windowBounds = [m_info objectForKey:(id)kCGWindowBounds];
    CGRect bounds = NSZeroRect;
    CGRectMakeWithDictionaryRepresentation(reinterpret_cast<CFDictionaryRef>(windowBounds), &bounds);
    retVal.x = bounds.origin.x;
    retVal.y = bounds.origin.y;
  }
  return retVal;
}

OSSize OSWindowMac::GetSize(void) {
  OSSize retVal;
  retVal.width = 0;
  retVal.height = 0;
  @autoreleasepool {
    NSDictionary* windowBounds = [m_info objectForKey:(id)kCGWindowBounds];
    CGRect bounds = NSZeroRect;
    CGRectMakeWithDictionaryRepresentation(reinterpret_cast<CFDictionaryRef>(windowBounds), &bounds);
    retVal.width = bounds.size.width;
    retVal.height = bounds.size.height;
  }
  return retVal;
}

void OSWindowMac::Cloak(void) {

}

void OSWindowMac::Uncloak(void) {
}

bool OSWindowMac::IsVisible(void) const {
  return true;
}
