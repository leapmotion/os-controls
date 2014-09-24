// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "AutoLaunch.h"
#include <CoreServices/CoreServices.h>

class AutoLaunchMac:
  public AutoLaunch
{
public:
  AutoLaunchMac();
  virtual ~AutoLaunchMac();

  virtual bool IsAutoLaunch() override;
  virtual bool SetAutoLaunch(bool shouldLaunch) override;

  static std::string GetAppPath();
  static std::string GetAppURL();
private:
  bool AddAutoLaunch();
  bool RemoveAutoLaunch();

  static void loginItemsChanged(LSSharedFileListRef lsRef, void* context);

  LSSharedFileListRef m_lsRef;
};
