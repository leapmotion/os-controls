#pragma once
#include "OSWindowMonitor.h"

class OSWindowMonitorWin:
  public OSWindowMonitor
{
public:
  OSWindowMonitorWin(void);
  ~OSWindowMonitorWin(void);

private:
  mutable std::mutex m_lock;

  // Current collection of known top-level windows
  std::unordered_map<HWND, std::shared_ptr<OSWindow>> m_knownWindows;

public:
  // OSWindowMonitor overrides:
  void Enumerate(std::function<void(OSWindow&)>& callback) const override;
};

