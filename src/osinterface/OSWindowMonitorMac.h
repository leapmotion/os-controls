#pragma once
#include "OSWindowMonitor.h"

class OSWindowEvent;
class OSWindowMac;

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
  typedef std::unordered_map<CGWindowID, std::shared_ptr<OSWindowMac>> t_knownWindows;
  t_knownWindows m_knownWindows;

protected:
  // OSWindowMonitor overrides:
  void Scan() override;

public:
  // OSWindowMonitor overrides:
  OSWindow* WindowFromPoint(const OSPoint& pt) const override;
  void Enumerate(const std::function<void(OSWindow&)>& callback) const override;
};
