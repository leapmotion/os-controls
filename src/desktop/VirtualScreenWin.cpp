// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "VirtualScreenWin.h"

namespace leap {

VirtualScreen* VirtualScreen::New(void)
{
  return new VirtualScreenWin;
}
//
// VirtualScreenHelperClass
//


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
    return reinterpret_cast<VirtualScreenWin*>(val)->WndProc(uMsg, wParam, lParam);
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//
// VirtualScreenWin
//

VirtualScreenWin::VirtualScreenWin()
{
  m_hWnd = ::CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                             MAKEINTRESOURCEW(VirtualScreenHelperClass::GetAtom()),
                             L"", WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, nullptr, nullptr, nullptr, this);
  ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
  ::ShowWindow(m_hWnd, SW_HIDE);
  Update();
}

VirtualScreenWin::~VirtualScreenWin()
{
  if (m_hWnd) {
    ::DestroyWindow(m_hWnd);
  }
}

std::vector<Screen> VirtualScreenWin::GetScreens() const
{
  std::vector<Screen> screens;
  EnumDisplayMonitors(0, 0, EnumerateDisplays, reinterpret_cast<LPARAM>(&screens));
  return screens;
}

LRESULT VirtualScreenWin::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_DISPLAYCHANGE) {
    Update();
  }
  return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK VirtualScreenWin::EnumerateDisplays(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
  std::vector<Screen>& screens = *reinterpret_cast<std::vector<Screen>*>(dwData);
  screens.push_back(Screen(hMonitor));
  return true;
}

}
