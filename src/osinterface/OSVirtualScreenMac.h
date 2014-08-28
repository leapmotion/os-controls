// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "OSVirtualScreen.h"

class OSVirtualScreenMac :
  public OSVirtualScreen
{
  public:
    OSVirtualScreenMac();
    virtual ~OSVirtualScreenMac();

  protected:
    virtual std::vector<OSScreen> GetScreens() const override;

  private:
    static void ConfigurationChangeCallback(CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags, void *that);
};
