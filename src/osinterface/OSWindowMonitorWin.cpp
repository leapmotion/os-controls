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

OSWindow* OSWindowMonitorWin::WindowFromPoint(const OSPoint& pt) const {
  // See what's under the specified point:
  HWND hwnd = ::WindowFromPoint(POINT{(int) pt.x, (int) pt.y});
  if(!hwnd)
    return nullptr;

  // Try to find the window in our table of top-level windows:
  std::lock_guard<std::mutex> lk(m_lock);
  auto wnd = m_knownWindows.find(hwnd);
  if(wnd == m_knownWindows.end())
    return nullptr;
  return wnd->second.get();
}

void OSWindowMonitorWin::Enumerate(const std::function<void(OSWindow&)>& callback) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for(auto& knownWindow : m_knownWindows)
    callback(*knownWindow.second);
}

struct enum_cblock {
  std::unordered_map<HWND, int> hwnds;
  int index;
};

void OSWindowMonitorWin::Scan() {
  enum_cblock block;
  block.index = 0;

  // Enumerate all top-level windows that we know about right now:
  EnumWindows(
    [] (HWND hwnd, LPARAM lParam) -> BOOL {
      auto& block = *(enum_cblock*) lParam;

      // Short-circuit if this window can't be seen
      if(!IsWindowVisible(hwnd))
        return true;

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
        block.hwnds[hwnd] = block.index--;
      return true;
    },
    (LPARAM) &block
  );

  // Figure out which windows are gone:
  std::unordered_map<HWND, std::shared_ptr<OSWindowWin>> pending;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(auto knownWindow : m_knownWindows) {
      auto q = block.hwnds.find(knownWindow.first);
      if(q == block.hwnds.end())
        // Window was gone the last time we enumerated, give up
        pending.insert(knownWindow);
      else
        // Found this window, update its z-order
        knownWindow.second->SetZOrder(q->second);
    }

    for(auto q : pending)
      m_knownWindows.erase(q.first);
  }

  // Fire notifications off while outside of the lock:
  for(auto q : pending)
    m_oswe(&OSWindowEvent::OnDestroy)(*q.second);
  pending.clear();

  // Create any windows which have been added:
  for(auto q : block.hwnds)
    if(!m_knownWindows.count(q.first)) {
      auto wnd = std::make_shared<OSWindowWin>(q.first);
      wnd->SetZOrder(q.second);
      pending[q.first] = wnd;
    }

  // Add to the collection:
  std::lock_guard<std::mutex>(m_lock),
  m_knownWindows.insert(pending.begin(), pending.end());

  // Creation notifications now
  for(auto q : pending)
    m_oswe(&OSWindowEvent::OnCreate)(*q.second);
}
