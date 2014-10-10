#include "stdafx.h"
#include "NativeWindow.h"

void NativeWindow::RaiseWindowAtPosition(float x, float y) {
  HWND hwnd = ::WindowFromPoint(POINT{(int) x, (int) y});
  if(!hwnd)
    return;
  ::SetForegroundWindow(hwnd);
}
