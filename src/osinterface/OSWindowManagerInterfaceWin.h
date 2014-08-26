#pragma once
#include "OSWindowManagerInterface.h"
#include "OSWindowWin.h"
#include "uievents/Updatable.h"

class OSWindowManagerInterfaceWin:
  public OSWindowManagerInterface,
  public Updatable
{
public:
  OSWindowManagerInterfaceWin(void);
  ~OSWindowManagerInterfaceWin(void);

public:
  // Updatable overrides:
  void Tick(std::chrono::duration<double> deltaT) override;

  // OSWindowManagerInterface overrides
  std::shared_ptr<OSWindow> GetForegroundWindow(void) override;
  std::vector<std::shared_ptr<OSApp>> EnumerateInteractiveApplications(void) override;
  std::vector<std::shared_ptr<OSWindow>> EnumerateVisibleTopLevelWindows(void) override;
};

