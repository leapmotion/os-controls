// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSAppMac.h"

#include <AppKit/NSRunningApplication.h>
#include <Foundation/NSData.h>
#include <Foundation/NSString.h>
#include <Foundation/NSURL.h>

OSAppMac::OSAppMac(uint32_t pid) : OSApp(pid)
{
  @autoreleasepool {
    NSRunningApplication* runningApp = [NSRunningApplication runningApplicationWithProcessIdentifier:pid];
    const char* name = [[runningApp localizedName] UTF8String];
    if (name) {
      m_name = std::string(name);
    }
    m_icon = [[runningApp icon] retain];
  }
}

OSAppMac::~OSAppMac()
{
  [m_icon release];
}

std::wstring OSApp::GetAppIdentifier(uint32_t pid) {
  std::wstring appPath;

  @autoreleasepool {
    NSRunningApplication* runningApp = [NSRunningApplication runningApplicationWithProcessIdentifier:pid];
    NSString* url = [[runningApp executableURL] absoluteString];
    if (url) {
      static_assert(sizeof(wchar_t) == 4, "Expecting 32-bit wchar_t type");
      NSData* data = [url dataUsingEncoding:NSUTF32LittleEndianStringEncoding];
      appPath = std::wstring(reinterpret_cast<const wchar_t*>([data bytes]), [data length]/sizeof(wchar_t));
    }
  }
  return appPath;
}

OSApp* OSApp::New(uint32_t pid) {
  return new OSAppMac(pid);
}

std::string OSAppMac::GetAppName(void) const {
  return m_name;
}

std::shared_ptr<ImagePrimitive> OSAppMac::GetIconTexture(std::shared_ptr<ImagePrimitive> img) const {
  if (!m_icon) {
    return img;
  }
  // FIXME
  return img;
}
