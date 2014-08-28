#include "stdafx.h"
#include "OSWindowMonitorWin.h"
#include "OSWindow.h"
#include "OSWindowEvent.h"

OSWindowMonitorWin::OSWindowMonitorWin(void)
{
}

OSWindowMonitorWin::~OSWindowMonitorWin(void)
{
}

OSWindowMonitor* OSWindowMonitor::New(void) {
  return new OSWindowMonitorWin;
}

void OSWindowMonitorWin::Enumerate(const std::function<void(OSWindow&)>& callback) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for(const auto& knownWindow : m_knownWindows)
    callback(*knownWindow.second);
}