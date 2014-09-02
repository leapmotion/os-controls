#include "stdafx.h"
#include "OSWindowMonitorWin.h"
#include "OSWindowWin.h"
#include "OSWindowEvent.h"

OSWindowMonitorWin::OSWindowMonitorWin(void)
{
  // Enumerate all top-level windows that we know about right now:
  std::lock_guard<std::mutex> lk(m_lock);
  EnumWindows(
    [] (HWND hwnd, LPARAM lParam) -> BOOL {
      ((OSWindowMonitorWin*) lParam)->m_knownWindows[hwnd] = std::make_shared<OSWindowWin>(hwnd);
      return true;
    },
    (LPARAM) this
  );
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