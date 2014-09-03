#include "stdafx.h"
#include "OSWindowMonitorWin.h"
#include "OSWindowWin.h"
#include "OSWindowEvent.h"

OSWindowMonitorWin::OSWindowMonitorWin(void)
{
  // Trigger initial enumeration
  Tick(std::chrono::duration<double>(0.0));
}

OSWindowMonitorWin::~OSWindowMonitorWin(void)
{
}

OSWindowMonitor* OSWindowMonitor::New(void) {
  return new OSWindowMonitorWin;
}

void OSWindowMonitorWin::Enumerate(const std::function<void(OSWindow&)>& callback) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for(auto& knownWindow : m_knownWindows)
    callback(*knownWindow.second);
}

void OSWindowMonitorWin::Tick(std::chrono::duration<double> deltaT) {
  // Enumerate all top-level windows that we know about right now:
  std::unordered_set<HWND> hwnds;
  EnumWindows(
    [] (HWND hwnd, LPARAM lParam) -> BOOL {
      // See if we are the last active visible popup
      HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);

      // Do not try to enumerate anything we own
      DWORD pid = 0;
      GetWindowThreadProcessId(hwnd, &pid);
      if(pid == GetCurrentProcessId())
        return true;
      
      for(HWND hwndTry = hwndWalk; hwndTry; ) {
        // Advance to the next spot:
        std::tie(hwndTry, hwndWalk) = std::make_tuple(GetLastActivePopup(hwndWalk), hwndTry);

        if(hwndTry == hwndWalk)
          // We haven't moved, we can end here
          break;

        if(IsWindowVisible(hwndTry))
          // Popup is visible, end here
          break;
      }

      if(hwndWalk == hwnd)
        // Add this window in, it would appear in the alt-tab list:
        ((std::unordered_set<HWND>*) lParam)->insert(hwnd);
      return true;
    },
    (LPARAM) &hwnds
  );

  // Figure out which windows are gone:
  std::unordered_map<HWND, std::shared_ptr<OSWindowWin>> pending;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(auto knownWindow : m_knownWindows)
      if(!hwnds.count(knownWindow.first))
        pending.insert(knownWindow);

    for(auto q : pending)
      m_knownWindows.erase(q.first);
  }

  // Fire notifications off while outside of the lock:
  for(auto q : pending)
    m_oswe(&OSWindowEvent::OnDestroy)(*q.second);
  pending.clear();

  // Create any windows which have been added:
  for(HWND hwnd : hwnds)
    if(!m_knownWindows.count(hwnd))
      pending[hwnd] = std::make_shared<OSWindowWin>(hwnd);

  // Add to the collection:
  std::lock_guard<std::mutex>(m_lock),
  m_knownWindows.insert(pending.begin(), pending.end());

  // Creation notifications now
  for(auto q : pending)
    m_oswe(&OSWindowEvent::OnCreate)(*q.second);
}