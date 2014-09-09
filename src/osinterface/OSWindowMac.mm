// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSWindowMac.h"
#include <Primitives.h>
#include <GLTexture2.h>

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

uint32_t OSWindowMac::GetOwnerPid(void) {
  return static_cast<uint32_t>([[m_info objectForKey:(id)kCGWindowOwnerPID] intValue]);
}

std::shared_ptr<OSApp> OSWindowMac::GetOwnerApp(void) {
  // FIXME
  return nullptr;
}

std::shared_ptr<ImagePrimitive> OSWindowMac::GetWindowTexture(std::shared_ptr<ImagePrimitive> img)  {
  CGImageRef imageRef = CGWindowListCreateImage(CGRectNull, kCGWindowListOptionIncludingWindow,
                                                m_windowID, kCGWindowImageNominalResolution);
  if (imageRef) {
    CFDataRef dataRef = CGDataProviderCopyData(CGImageGetDataProvider(imageRef));
    if (dataRef) {
      const uint8_t* dstBytes = CFDataGetBytePtr(dataRef);
      const size_t bytesPerRow = CGImageGetBytesPerRow(imageRef);
      // const size_t width = CGImageGetWidth(imageRef);
      // For now, adjust the width to be that of the stride -- FIXME
      assert(bytesPerRow % 4 == 0);
      const size_t width = bytesPerRow/4;
      const size_t height = CGImageGetHeight(imageRef);
      const size_t totalBytes = bytesPerRow*height;
      GLTexture2Params params{static_cast<GLsizei>(width), static_cast<GLsizei>(height)};
      params.SetTarget(GL_TEXTURE_2D);
      params.SetInternalFormat(GL_RGBA8);
      params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      GLTexture2PixelDataReference pixelData{GL_BGRA, GL_UNSIGNED_BYTE, dstBytes, totalBytes};

      // If we can re-use the passed in image primitive do that, if not create new one -- FIXME

      img = std::make_shared<ImagePrimitive>(std::make_shared<GLTexture2>(params, pixelData));
      CFRelease(dataRef);
    }
    CFRelease(imageRef);
  }
  return img;
}

bool OSWindowMac::GetFocus(void) {
  // FIXME
  return false;
}

void OSWindowMac::SetFocus(void) {
  const pid_t pid = static_cast<pid_t>([[m_info objectForKey:(id)kCGWindowOwnerPID] intValue]);
  if (!pid) {
    return;
  }
  // An AppleScript implementation
  @autoreleasepool {
    // First make the window the top-level window for the application
    std::ostringstream oss;
    oss << "tell application \"System Events\"\n"
        << "\tset appName to name of item 1 of (processes whose unix id is " << pid << ")\n"
        << "\ttell my application appName\n"
        << "\t\tset theWindow to window id " << m_windowID << "\n"
        << "\t\ttell theWindow\n"
        << "\t\t\tif index of theWindow is not 1 then\n"
        << "\t\t\t\tset index to 1\n"
        << "\t\t\t\tset visible to false\n"
        << "\t\t\tend if\n"
        << "\t\t\tset visible to true\n"
        << "\t\tend tell\n"
        << "\tend tell\n"
        << "end tell\n";
    NSString* script = [NSString stringWithUTF8String:oss.str().c_str()];
    NSAppleScript* as = [[NSAppleScript alloc] initWithSource:script];
    NSDictionary* errInfo;
    NSAppleEventDescriptor* res = [as executeAndReturnError:&errInfo];
    // Then bring the application to front
    [[NSRunningApplication runningApplicationWithProcessIdentifier:pid]
     activateWithOptions:NSApplicationActivateIgnoringOtherApps];
  }
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
  // FIXME
}

void OSWindowMac::Uncloak(void) {
  // FIXME
}

bool OSWindowMac::IsVisible(void) const {
  // FIXME
  return true;
}
