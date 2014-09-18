#pragma once
#include "HtmlPageLauncher.h"

class HtmlPageLauncherWin:
  public HtmlPageLauncher
{
public:
  HtmlPageLauncherWin(void);
  ~HtmlPageLauncherWin(void);

  // HtmlPageLauncher overrides
  void LaunchPage(const char* url) override;
};

