// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "VirtualScreenMac.h"

namespace leap {

VirtualScreen* VirtualScreen::New(void)
{
  return new VirtualScreenMac;
}

//
// VirtualScreenMac
//

VirtualScreenMac::VirtualScreenMac()
{
  CGDisplayRegisterReconfigurationCallback(ConfigurationChangeCallback, this);
  Update();
}

VirtualScreenMac::~VirtualScreenMac()
{
  CGDisplayRemoveReconfigurationCallback(ConfigurationChangeCallback, this);
}

// Called when the display configuration changes
void VirtualScreenMac::ConfigurationChangeCallback(CGDirectDisplayID display,
                                                   CGDisplayChangeSummaryFlags flags,
                                                   void *that)
{
  if ((flags & kCGDisplayBeginConfigurationFlag) == 0 && that) {
    static_cast<VirtualScreenMac*>(that)->Update();
  }
}

std::vector<Screen> VirtualScreenMac::GetScreens() const
{
  std::vector<Screen> screens;
  uint32_t numDisplays = 0;

  if (CGGetActiveDisplayList(0, 0, &numDisplays) == kCGErrorSuccess && numDisplays > 0) {
    CGDirectDisplayID *screenIDs = new CGDirectDisplayID[numDisplays];

    if (screenIDs) {
      if (CGGetActiveDisplayList(numDisplays, screenIDs, &numDisplays) == kCGErrorSuccess) {
        for (int i = 0; i < numDisplays; i++) {
          screens.push_back(Screen(screenIDs[i]));
        }
      }
      delete [] screenIDs;
    }
  }
  if (screens.empty()) {
    screens.push_back(Screen(CGMainDisplayID()));
  }
  return screens;
}

}
