// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include <string>

//Governs this application launching on startup
class AutoLaunch:
  public ContextMember
{
public:
  AutoLaunch() {}
  virtual ~AutoLaunch() {}

  static AutoLaunch* New(void);

  void SetFriendlyName(const std::string& appName) { m_appName = appName; }
  void SetCommandLineOptions(const std::string& commandLineOptions) { m_commandLineOptions = commandLineOptions; }

  virtual bool IsAutoLaunch() = 0;
  virtual bool SetAutoLaunch(bool shouldLaunch) = 0;
protected:
  std::string m_appName;
  std::string m_commandLineOptions;
};
