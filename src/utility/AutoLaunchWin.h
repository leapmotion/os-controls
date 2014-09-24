// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "AutoLaunch.h"

class AutoLaunchWin:
  public AutoLaunch
{
public:
  AutoLaunchWin();

  virtual bool IsAutoLaunch() override;
  virtual bool SetAutoLaunch(bool shouldLaunch) override;
private:
  std::string m_command;
};
