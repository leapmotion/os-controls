// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "VirtualScreen.h"

namespace leap  {

class VirtualScreenMac :
  public VirtualScreen
{
  public:
    VirtualScreenMac();
    virtual ~VirtualScreenMac();

  protected:
    virtual std::vector<Screen> GetScreens() const override;

  private:
    static void ConfigurationChangeCallback(CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags, void *that);
};

}
