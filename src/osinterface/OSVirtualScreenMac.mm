// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSVirtualScreenMac.h"

OSVirtualScreen* OSVirtualScreen::New(void)
{
  return new OSVirtualScreenMac;
}

//
// OSVirtualScreenMac
//

OSVirtualScreenMac::OSVirtualScreenMac()
{
  CGDisplayRegisterReconfigurationCallback(ConfigurationChangeCallback, this);
  Update();
}

OSVirtualScreenMac::~OSVirtualScreenMac()
{
  CGDisplayRemoveReconfigurationCallback(ConfigurationChangeCallback, this);
}

// Called when the display configuration changes
void OSVirtualScreenMac::ConfigurationChangeCallback(CGDirectDisplayID display,
                                                     CGDisplayChangeSummaryFlags flags,
                                                     void *that)
{
  if ((flags & kCGDisplayBeginConfigurationFlag) == 0 && that) {
    static_cast<OSVirtualScreenMac*>(that)->Update();
  }
}

std::vector<OSScreen> OSVirtualScreenMac::GetScreens() const
{
  std::vector<OSScreen> screens;
  uint32_t numDisplays = 0;

  if (CGGetActiveDisplayList(0, 0, &numDisplays) == kCGErrorSuccess && numDisplays > 0) {
    CGDirectDisplayID *screenIDs = new CGDirectDisplayID[numDisplays];

    if (screenIDs) {
      if (CGGetActiveDisplayList(numDisplays, screenIDs, &numDisplays) == kCGErrorSuccess) {
        for (int i = 0; i < numDisplays; i++) {
          screens.push_back(OSScreen(screenIDs[i]));
        }
      }
      delete [] screenIDs;
    }
  }
  if (screens.empty()) {
    screens.push_back(OSScreen(CGMainDisplayID()));
  }
  return screens;
}
