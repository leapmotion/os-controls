#include "stdafx.h"
#include "OSWindowWin.h"
#include <Primitives.h>

OSWindowWin::OSWindowWin(HWND hwnd):
  hwnd(hwnd)
{
}

OSWindowWin::~OSWindowWin(void)
{
}

bool OSWindowWin::IsValid(void) {
  return !!IsWindow(hwnd);
}

std::shared_ptr<OSApp> OSWindowWin::GetOwnerApp(void) {
  return nullptr;
}

uint32_t OSWindowWin::GetOwnerPid(void) {
  DWORD pid;
  GetWindowThreadProcessId(hwnd, &pid);
  return pid;
}

void OSWindowWin::GetWindowTexture(ImagePrimitive& texture) {
  return OSWindow::GetWindowTexture(texture);
}

bool OSWindowWin::GetFocus(void) {
  HWND foreground = GetForegroundWindow();
  return !!IsChild(foreground, hwnd);
}

void OSWindowWin::SetFocus(void) {
  ::SetForegroundWindow(hwnd);
}

std::wstring OSWindowWin::GetTitle(void) {
  std::wstring retVal(256, 0);
  int nch = GetWindowTextW(hwnd, &retVal[0], retVal.size());
  retVal.resize(nch);
  return retVal;
}

OSPoint OSWindowWin::GetPosition(void) {
  RECT rect;
  GetWindowRect(hwnd, &rect);

  OSPoint retVal;
  retVal.x = (float) rect.left;
  retVal.y = (float) rect.top;
  return retVal;
}

OSSize OSWindowWin::GetSize(void) {
  RECT rect;
  GetWindowRect(hwnd, &rect);

  OSSize retVal;
  retVal.width = (float) (rect.right - rect.left);
  retVal.height = (float) (rect.bottom - rect.top);
  return retVal;
}

void OSWindowWin::Cloak(void) {

}

void OSWindowWin::Uncloak(void) {
}

bool OSWindowWin::IsVisible(void) const {
  return !!::IsWindowVisible(hwnd);
}
