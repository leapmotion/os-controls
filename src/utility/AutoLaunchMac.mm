// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutoLaunchMac.h"

#import <Cocoa/Cocoa.h>

AutoLaunch* AutoLaunch::New(void)
{
  return new AutoLaunchMac;
}

AutoLaunchMac::AutoLaunchMac() :
  m_lsRef(LSSharedFileListCreate(nullptr, kLSSharedFileListSessionLoginItems, nullptr))
{
  if (m_lsRef) {
    LSSharedFileListAddObserver(m_lsRef,
                                CFRunLoopGetMain(),
                                kCFRunLoopCommonModes,
                                loginItemsChanged,
                                this);
  }
}

AutoLaunchMac::~AutoLaunchMac()
{
  if (!m_lsRef) {
    return;
  }
  LSSharedFileListRemoveObserver(m_lsRef,
                                 CFRunLoopGetMain(),
                                 kCFRunLoopCommonModes,
                                 loginItemsChanged,
                                 this);
  CFRelease(m_lsRef);
}

bool AutoLaunchMac::IsAutoLaunch()
{
  if (!m_lsRef) {
    return false;
  }
  bool isAutoLaunch = false;
  const std::string appURL = getAppURL();

  CFArrayRef arrayRef = LSSharedFileListCopySnapshot(m_lsRef, nullptr);
  CFIndex count = CFArrayGetCount(arrayRef);
  for (auto index = 0; index < count; index++) {
    const LSSharedFileListItemRef itemRef = (const LSSharedFileListItemRef)CFArrayGetValueAtIndex(arrayRef, index);
    if (!itemRef) {
      continue;
    }
    CFURLRef urlRef = nullptr;
    if (LSSharedFileListItemResolve(itemRef, 0, &urlRef, nullptr) != noErr) {
      continue;
    }
    CFIndex length = CFURLGetBytes(urlRef, nullptr, 0);
    std::unique_ptr<UInt8[]> buffer(new UInt8[length]);
    if (buffer && CFURLGetBytes(urlRef, buffer.get(), length) == length) {
      std::string url(reinterpret_cast<const char *>(buffer.get()), length);
      if (url == appURL) {
        isAutoLaunch = true;
      }
    }
    CFRelease(urlRef);
    if (isAutoLaunch) {
      break;
    }
  }
  CFRelease(arrayRef);

  return isAutoLaunch;
}

bool AutoLaunchMac::SetAutoLaunch(bool shouldLaunch)
{
  return shouldLaunch ? AddAutoLaunch() : RemoveAutoLaunch();
}

bool AutoLaunchMac::AddAutoLaunch()
{
  if (!m_lsRef) {
    return false;
  }
  std::string url = getAppURL();
  CFURLRef urlRef = CFURLCreateWithBytes(nullptr, (const UInt8*)(url.c_str()), (CFIndex)url.size(), kCFStringEncodingUTF8, nullptr);
  if (!urlRef) {
    return false;
  }
  LSSharedFileListItemRef itemRef = LSSharedFileListInsertItemURL(m_lsRef,
                                                                  kLSSharedFileListItemLast, nullptr, nullptr,
                                                                  urlRef, nullptr, nullptr);
  CFRelease(urlRef);
  if (!itemRef) {
    return false;
  }
  CFRelease(itemRef);
  return true;
}

bool AutoLaunchMac::RemoveAutoLaunch()
{
  if (!m_lsRef) {
    return false;
  }
  bool removedAutoLaunch = false;
  const std::string appURL = getAppURL();

  CFArrayRef arrayRef = LSSharedFileListCopySnapshot(m_lsRef, nullptr);
  CFIndex count = CFArrayGetCount(arrayRef);
  for (auto index = 0; index < count; index++) {
    const LSSharedFileListItemRef itemRef = (const LSSharedFileListItemRef)CFArrayGetValueAtIndex(arrayRef, index);
    if (!itemRef) {
      continue;
    }
    CFURLRef urlRef = nullptr;
    if (LSSharedFileListItemResolve(itemRef, 0, &urlRef, nullptr) != noErr) {
      continue;
    }
    CFIndex length = CFURLGetBytes(urlRef, nullptr, 0);
    std::unique_ptr<UInt8[]> buffer(new UInt8[length]);
    if (buffer && CFURLGetBytes(urlRef, buffer.get(), length) == length) {
      std::string url(reinterpret_cast<const char *>(buffer.get()), length);
      if (url == appURL) {
        removedAutoLaunch = (LSSharedFileListItemRemove(m_lsRef, itemRef) == noErr);
      }
    }
    CFRelease(urlRef);
    if (removedAutoLaunch) {
      break;
    }
  }
  CFRelease(arrayRef);

  return removedAutoLaunch;
}

std::string AutoLaunchMac::getAppURL()
{
  @autoreleasepool {
    NSString* path = [@"~/Applications/AirspaceApps/Shortcuts.app" stringByExpandingTildeInPath];
    NSBundle* bundle = [NSBundle bundleForClass:NSClassFromString(@"ComLeapMotionShortcutsPreferencePane")];
    NSString* shortcutsPrefPanePath = [bundle bundlePath];
    NSString* expectedEnd = @"/Shortcuts.app/Contents/MacOS/Shortcuts.prefPane";
    NSUInteger length = shortcutsPrefPanePath.length;

    if (length >= expectedEnd.length) {
      path = [shortcutsPrefPanePath substringToIndex:(length - expectedEnd.length + 14)]; // (+ 14 => "/Shortcuts.app")
    }
    NSString* resolvedPath = [[NSFileManager defaultManager] destinationOfSymbolicLinkAtPath:path error:nil];
    if (resolvedPath) {
      path = resolvedPath;
    }
    return std::string([[[NSURL fileURLWithPath:path] absoluteString] UTF8String]);
  }
}

void AutoLaunchMac::loginItemsChanged(LSSharedFileListRef lsRef, void* context)
{
  AutoLaunchMac* that = reinterpret_cast<AutoLaunchMac*>(context);
  if (that) {
    // FIXME
  }
}
