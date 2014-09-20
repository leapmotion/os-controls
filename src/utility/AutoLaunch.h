#pragma once

#include <string>

//Governs this application launching on startup
class AutoLaunch
{
public:
  AutoLaunch(const char* appName, const char* commandLineOptions = nullptr);

  bool IsAutoLaunch();
  void SetAutoLaunch();
private:
  std::string m_appName;
  std::string m_command;
};

