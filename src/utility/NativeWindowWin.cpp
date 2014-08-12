#include "stdafx.h"
#include "NativeWindow.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

void NativeWindow::MakeTransparent(const Handle window) {
  if (!window) {
    throw std::runtime_error("Error retrieving native window");
  }

  LONG flags = ::GetWindowLongA(window, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT;
  ::SetWindowLongA(window, GWL_EXSTYLE, flags);
  ::SetLayeredWindowAttributes(window, RGB(0, 0, 0), 255, LWA_ALPHA);
 
  DWM_BLURBEHIND bb = { 0 };
  bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
  bb.fEnable = true;
  bb.hRgnBlur = CreateRectRgn(0, 0, 1, 1);
  ::DwmEnableBlurBehindWindow(window, &bb);
}

void NativeWindow::MakeAlwaysOnTop(const Handle window) {
  if (window) {
    LONG flags = ::GetWindowLongA(window, GWL_EXSTYLE) | WS_EX_TOPMOST;;
    ::SetWindowLongA(window, GWL_EXSTYLE, flags);
  }
  else {
    throw std::runtime_error("Error retrieving native window");
  }
  ::SetWindowPos(window, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
}
