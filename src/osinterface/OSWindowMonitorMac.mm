#include "stdafx.h"
#include "OSWindowMonitorMac.h"
#include "OSWindowMac.h"
#include "OSWindowEvent.h"

#include <AppKit/NSWindow.h>
#include <cfloat>

OSWindowMonitorMac::OSWindowMonitorMac(void)
{
  // Trigger initial enumeration
  Scan();
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

void OSWindowMonitorMac::Scan() {
  static int s_pid = static_cast<int>(getpid());
  const uint32_t mark = ++m_mark;
  int zOrder = 0;
  std::unique_lock<std::mutex> lk(m_lock);
  size_t previousCount = m_knownWindows.size();
  lk.unlock();

  @autoreleasepool {
    NSArray* windowArray =
        (id)CFBridgingRelease(CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly |
                                                         kCGWindowListExcludeDesktopElements, kCGNullWindowID));
    // Loop through the windows
    for (NSDictionary* entry in windowArray) {
      // Do additional filtering of windows
      if ([[entry objectForKey:(id)kCGWindowLayer] intValue] != 0 ||
          [[entry objectForKey:(id)kCGWindowAlpha] floatValue] < FLT_EPSILON ||
           [entry objectForKey:(id)kCGWindowOwnerName] == nil ||
          [[entry objectForKey:(id)kCGWindowOwnerPID] intValue] == s_pid ||
          [[entry objectForKey:(id)kCGWindowSharingState] intValue] == kCGWindowSharingNone) {
        continue;
      }
      const CGWindowID windowID = [[entry objectForKey:(id)kCGWindowNumber] unsignedIntValue];
      if (windowID == 0) {
        continue;
      }
      lk.lock();
      // See if we already know about this window.
      auto found = m_knownWindows.find(windowID);
      if (found == m_knownWindows.end()) {
        auto window = std::make_shared<OSWindowMac>(entry);
        window->SetMark(mark);
        window->SetZOrder(zOrder--);
        m_knownWindows[windowID] = window;
        lk.unlock();
        // Fire notifications off while outside of the lock:
        m_oswe(&OSWindowEvent::OnCreate)(*window);
      } else {
        auto& window = found->second;
        auto prvSize = window->GetSize();
        window->UpdateInfo(entry);
        window->SetMark(mark);
        window->SetZOrder(zOrder--);
        --previousCount; // Saw this window last time, decrement the count
        auto newSize = window->GetSize();
        const bool wasResized = newSize.height != prvSize.height || newSize.width != prvSize.width;
        lk.unlock();
        if (wasResized) {
          m_oswe(&OSWindowEvent::OnResize)(*window);
        }
      }
    }
  }
  // If we can account for all of the previously seen windows, there is no need to check for destroyed windows.
  if (previousCount == 0) {
    return;
  }

  // Sweep through the known windows to find those that are not marked as expected
  std::vector<std::shared_ptr<OSWindowMac>> pending;
  lk.lock();
  for (auto knownWindow = m_knownWindows.begin(); knownWindow != m_knownWindows.end(); ) {
    auto& window = knownWindow->second;
    if (window->Mark() != mark) {
      pending.push_back(window);
      knownWindow = m_knownWindows.erase(knownWindow);
    } else {
      ++knownWindow;
    }
  }
  lk.unlock();
  // Fire notifications off while outside of the lock:
  for (auto& window : pending) {
    m_oswe(&OSWindowEvent::OnDestroy)(*window);
  }
}
