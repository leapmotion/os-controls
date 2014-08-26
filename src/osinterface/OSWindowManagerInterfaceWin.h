#pragma once
#include "OSWindowManagerInterface.h"
#include "OSWindowWin.h"

class OSWindowManagerInterfaceWin:
  public OSWindowManagerInterface
{
public:
  OSWindowManagerInterfaceWin(void);
  ~OSWindowManagerInterfaceWin(void);

public:
  // OSWindowManagerInterface overrides
  std::shared_ptr<OSWindow> GetForegroundWindow(void) override;
  std::vector<std::shared_ptr<OSApp>> EnumerateInteractiveApplications(void) override;
  std::vector<std::shared_ptr<OSWindow>> EnumerateVisibleTopLevelWindows(void) override;
};

