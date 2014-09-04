#include "stdafx.h"
#include "OSWindowMonitorMac.h"
#include "OSWindowMac.h"
#include "OSWindowEvent.h"

#include <AppKit/NSWindow.h>
#include <cfloat>

OSWindowMonitorMac::OSWindowMonitorMac(void)
{
  // Trigger initial enumeration
  Tick(std::chrono::duration<double>(0.0));
}

OSWindowMonitorMac::~OSWindowMonitorMac(void)
{
}

OSWindowMonitor* OSWindowMonitor::New(void) {
  return new OSWindowMonitorMac;
}

OSWindow* OSWindowMonitorMac::WindowFromPoint(const OSPoint& pt) const {
  return nullptr;
}

void OSWindowMonitorMac::Enumerate(const std::function<void(OSWindow&)>& callback) const {
  std::lock_guard<std::mutex> lk(m_lock);
  for(const auto& knownWindow : m_knownWindows)
    callback(*knownWindow.second);
}

void OSWindowMonitorMac::Scan() {
  static pid_t s_pid = getpid();
  std::unordered_set<CGWindowID> windows;
  @autoreleasepool {
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly |
                                                       kCGWindowListExcludeDesktopElements, kCGNullWindowID);
    NSArray* windowArray = CFBridgingRelease(windowList);
    for (NSDictionary* entry in windowArray) {
      const int sharingState = [[entry objectForKey:(id)kCGWindowSharingState] intValue];
      if (sharingState == kCGWindowSharingNone) {
        continue;
      }
      const int windowLayer = [[entry objectForKey:(id)kCGWindowLayer] intValue];
      if (windowLayer != 0) {
        continue;
      }
      const float windowAlpha = [[entry objectForKey:(id)kCGWindowAlpha] floatValue];
      if (windowAlpha < FLT_EPSILON) {
        continue;
      }
      NSString *applicationName = [entry objectForKey:(id)kCGWindowOwnerName];
      if (!applicationName) {
        continue;
      }
      const pid_t pid = static_cast<pid_t>([[entry objectForKey:(id)kCGWindowOwnerPID] intValue]);
      if (pid == s_pid) {
        continue;
      }
      const CGWindowID windowID = [[entry objectForKey:(id)kCGWindowNumber] unsignedIntValue];
      windows.insert(windowID);
    }
  }

  // Figure out which windows are gone:
  std::unordered_map<CGWindowID, std::shared_ptr<OSWindowMac>> pending;
  {
    std::lock_guard<std::mutex> lk(m_lock);
    for(auto knownWindow : m_knownWindows) {
      if(windows.find(knownWindow.first) == windows.end()) {
        pending.insert(knownWindow);
      }
    }
    for(auto q : pending) {
      m_knownWindows.erase(q.first);
    }
  }

  // Fire notifications off while outside of the lock:
  for(auto q : pending) {
    m_oswe(&OSWindowEvent::OnDestroy)(*q.second);
  }
  pending.clear();

  // Create any windows which have been added:
  for(CGWindowID windowID : windows) {
    if(!m_knownWindows.count(windowID)) {
      pending[windowID] = std::make_shared<OSWindowMac>(windowID);
    }
  }

  // Add to the collection:
  std::lock_guard<std::mutex>(m_lock),
  m_knownWindows.insert(pending.begin(), pending.end());

  // Creation notifications now
  for(auto q : pending) {
    m_oswe(&OSWindowEvent::OnCreate)(*q.second);
  }
}
