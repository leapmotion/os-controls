// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "Screen.h"
#include <vector>
#include <mutex>

namespace leap  {

class VirtualScreenListener {
  public:
    VirtualScreenListener(void) {}
    virtual ~VirtualScreenListener(void) {}

    virtual void OnChange(void) = 0;
};

class VirtualScreen : public ScreenBase {
  public:
    VirtualScreen();
    virtual ~VirtualScreen();

    std::vector<Screen> Screens() const {
      std::lock_guard<std::mutex> lock(m_mutex);
      return m_screens;
    }

    Screen PrimaryScreen() const;
    Screen ClosestScreen(const Point& position) const;

  private:
    void Update();

    Rect ComputeBounds(const std::vector<Screen>& screens);
    Point ClipPosition(const Point& position, uint32_t* index) const;

#if _WIN32
    friend class VirtualScreenHelperClass;

    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK EnumerateDisplays(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

    HWND m_hWnd;
#elif __APPLE__
    static void ConfigurationChangeCallback(CGDirectDisplayID display, CGDisplayChangeSummaryFlags flags, void *that);
#endif
    mutable std::mutex m_mutex;
    std::vector<Screen> m_screens;
};

#if _WIN32
class VirtualScreenHelperClass {
  public:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static ATOM GetAtom() { static VirtualScreenHelperClass s_instance; return s_instance.m_atom; }

  private:
    VirtualScreenHelperClass();
    ~VirtualScreenHelperClass();

    WNDCLASSW m_wndClass;
    ATOM m_atom;
};
#endif

}
