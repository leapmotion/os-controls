// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "VirtualScreen.h"

namespace leap {

//
// VirtualScreen
//

VirtualScreen::VirtualScreen()
{
#if __APPLE__
  CGDisplayRegisterReconfigurationCallback(ConfigurationChangeCallback, this);
#elif _WIN32
  m_hWnd = ::CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                             MAKEINTRESOURCEW(VirtualScreenHelperClass::GetAtom()),
                             L"", WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, nullptr, nullptr, nullptr, this);
  ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
  ::ShowWindow(m_hWnd, SW_HIDE);
#endif
  Update();
}

VirtualScreen::~VirtualScreen()
{
#if __APPLE__
  CGDisplayRemoveReconfigurationCallback(ConfigurationChangeCallback, this);
#elif _WIN32
  if (m_hWnd) {
    ::DestroyWindow(m_hWnd);
  }
#endif
}

#if __APPLE__
// Called when the the display configuration changes
void VirtualScreen::ConfigurationChangeCallback(CGDirectDisplayID display,
                                                CGDisplayChangeSummaryFlags flags,
                                                void *that)
{
  if ((flags & kCGDisplayBeginConfigurationFlag) == 0 && that) {
    static_cast<VirtualScreen*>(that)->Update();
  }
}
#endif

Screen VirtualScreen::PrimaryScreen() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  uint32_t numDisplays = static_cast<uint32_t>(m_screens.size());
  if (numDisplays > 1) {
    for (uint32_t i = 0; i < numDisplays; i++) {
      if (m_screens[i].IsPrimary()) {
        return m_screens[i];
      }
    }
  }
  if (m_screens.empty()) {
    throw std::runtime_error("Unable to detect screens");
  }
  return m_screens[0];
}

Screen VirtualScreen::ClosestScreen(const Point& position) const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  uint32_t numDisplays = static_cast<uint32_t>(m_screens.size());

  if (numDisplays > 1) {
    for (uint32_t i = 0; i < numDisplays; i++) {
      if (RectContainsPoint(m_screens[i].Bounds(), position)) {
        return m_screens[i];
      }
    }
    int bestIndex = 0;
    Float bestSquaredDistance = 0;
    for (uint32_t i = 0; i < numDisplays; i++) {
      Point clipped = m_screens[i].ClipPosition(position);
      const Float dx = (clipped.x - position.x);
      const Float dy = (clipped.y - position.y);
      Float squaredDistance = dx*dx + dy*dy;
      if (i == 0 || squaredDistance < bestSquaredDistance) {
        bestIndex = i;
        bestSquaredDistance = squaredDistance;
      }
    }
    return m_screens[bestIndex];
  }
  if (m_screens.empty()) {
    throw std::runtime_error("Unable to detect screens");
  }
  return m_screens[0];
}

void VirtualScreen::Update()
{
  std::vector<Screen> screens;
  uint32_t numDisplays = 0;

#if __APPLE__
  if (CGGetActiveDisplayList(0, 0, &numDisplays) == kCGErrorSuccess && numDisplays > 0) {
    CGDirectDisplayID *screenIDs = new CGDirectDisplayID[numDisplays];

    if (screenIDs) {
      if (CGGetActiveDisplayList(numDisplays, screenIDs, &numDisplays) == kCGErrorSuccess) {
        for (int i = 0; i < numDisplays; i++) {
          screens.push_back(Screen(screenIDs[i]));
        }
      }
      delete [] screenIDs;
    }
  }
  if (screens.empty()) {
    screens.push_back(Screen(CGMainDisplayID()));
    numDisplays = 1;
  }
#elif _WIN32
  EnumDisplayMonitors(0, 0, EnumerateDisplays, reinterpret_cast<LPARAM>(&screens));
  numDisplays = static_cast<uint32_t>(screens.size());
#else
  // Linux -- FIXME
#endif
  std::unique_lock<std::mutex> lock(m_mutex);
  m_screens = screens;
  m_bounds = ComputeBounds(m_screens);
  lock.unlock();
  AutoFired<VirtualScreenListener> vsl;
  vsl(&VirtualScreenListener::OnChange)();
}

Rect VirtualScreen::ComputeBounds(const std::vector<Screen>& screens)
{
  size_t numScreens = screens.size();

  if (numScreens == 1) {
    return screens[0].Bounds();
  } else if (numScreens > 1) {
    Rect bounds = screens[0].Bounds();

    for (size_t i = 1; i < numScreens; i++) {
      bounds = RectUnion(bounds, screens[i].Bounds());
    }
    return bounds;
  } else {
    return RectZero;
  }
}

#if _WIN32
LRESULT VirtualScreen::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_DISPLAYCHANGE) {
    Update();
  }
  return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK VirtualScreen::EnumerateDisplays(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
  std::vector<Screen>& screens = *reinterpret_cast<std::vector<Screen>*>(dwData);
  screens.push_back(Screen(hMonitor));
  return true;
}

//
// VirtualScreenHelperClass
//

VirtualScreenHelperClass::VirtualScreenHelperClass()
{
  m_wndClass.style = CS_NOCLOSE;
  m_wndClass.lpfnWndProc = WndProc;
  m_wndClass.cbWndExtra = sizeof(void*);
  m_wndClass.cbClsExtra = 0;
  m_wndClass.hInstance = nullptr;
  m_wndClass.hIcon = nullptr;
  m_wndClass.hCursor = nullptr;
  m_wndClass.hbrBackground = nullptr;
  m_wndClass.lpszMenuName = nullptr;
  m_wndClass.lpszClassName = L"Leap::Desktop";
  m_atom = RegisterClassW(&m_wndClass);
}

VirtualScreenHelperClass::~VirtualScreenHelperClass()
{
  UnregisterClassW(L"Leap::Desktop", nullptr);
}

LRESULT CALLBACK VirtualScreenHelperClass::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  LONG_PTR val = GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (val) {
    return reinterpret_cast<VirtualScreen*>(val)->WndProc(uMsg, wParam, lParam);
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif

}
