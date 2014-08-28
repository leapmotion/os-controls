#pragma once
#include "OSWindowMonitor.h"

class OSWindowEvent;

class OSWindowMonitorWin:
  public OSWindowMonitor
{
public:
  OSWindowMonitorWin(void);
  ~OSWindowMonitorWin(void);

private:
  mutable std::mutex m_lock;

  // OS window event:
  AutoFired<OSWindowEvent> m_oswe;

  // Current collection of known top-level windows
  std::unordered_map<HWND, std::shared_ptr<OSWindow>> m_knownWindows;

public:
  // OSWindowMonitor overrides:
  void Enumerate(const std::function<void(OSWindow&)>& callback) const override;
};

