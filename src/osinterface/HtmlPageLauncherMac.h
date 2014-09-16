#pragma once
#include "HtmlPageLauncher.h"

class HtmlPageLauncherMac:
  public HtmlPageLauncher
{
public:
  HtmlPageLauncherMac(void);
  ~HtmlPageLauncherMac(void);

  // HtmlPageLauncher overrides
  void LaunchPage(const char* url) override;
};

