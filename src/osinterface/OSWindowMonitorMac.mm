#include "stdafx.h"
#include "OSWindowMonitorMac.h"
#include "OSWindow.h"
#include "OSWindowEvent.h"

OSWindowMonitorMac::OSWindowMonitorMac(void)
{
  // Set up a hook so we can snag window creation events:

  // Enumerate all top-level windows that we know about right now:
}

OSWindowMonitorMac::~OSWindowMonitorMac(void)
{
}

OSWindowMonitor* OSWindowMonitor::New(void) {
  return new OSWindowMonitorMac;
}

void OSWindowMonitorMac::Enumerate(const std::function<void(OSWindow&)>& callback) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for(const auto& knownWindow : m_knownWindows)
    callback(*knownWindow.second);
}
