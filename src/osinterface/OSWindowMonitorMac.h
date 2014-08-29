#pragma once
#include "OSWindowMonitor.h"
#include <AppKit/NSWindow.h>

class OSWindowEvent;

class OSWindowMonitorMac:
  public OSWindowMonitor
{
public:
  OSWindowMonitorMac(void);
  ~OSWindowMonitorMac(void);

private:
  mutable std::mutex m_lock;

  // OS window event:
  AutoFired<OSWindowEvent> m_oswe;

  // Current collection of known top-level windows
  std::unordered_map<CGWindowID, std::shared_ptr<OSWindow>> m_knownWindows;

public:
  // OSWindowMonitor overrides:
  void Enumerate(const std::function<void(OSWindow&)>& callback) const override;
};

