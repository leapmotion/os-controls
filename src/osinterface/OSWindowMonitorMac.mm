#include "stdafx.h"
#include "OSWindowMonitorMac.h"
#include "OSWindow.h"
#include "OSWindowEvent.h"
#include "OSWindowMac.h"

OSWindowMonitorMac::OSWindowMonitorMac(void)
{
}

OSWindowMonitorMac::~OSWindowMonitorMac(void)
{
}

OSWindowMonitor* OSWindowMonitor::New(void) {
  return new OSWindowMonitorMac;
}

void OSWindowMonitorMac::Tick(std::chrono::duration<double> deltaT) {
}

void OSWindowMonitorMac::Enumerate(const std::function<void(OSWindow&)>& callback) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for(const auto& knownWindow : m_knownWindows)
    callback(*knownWindow.second);
}
