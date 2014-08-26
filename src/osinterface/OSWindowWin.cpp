#include "stdafx.h"
#include "OSWindowWin.h"

OSWindowWin::OSWindowWin(HWND hwnd):
  hwnd(hwnd)
{
}

OSWindowWin::~OSWindowWin(void)
{
}

std::vector<std::shared_ptr<OSWindowNode>> OSWindowWin::EnumerateChildren(void) {
  std::vector<std::shared_ptr<OSWindowNode>> retVal;
  EnumChildWindows(
    hwnd,
    [](HWND hwnd, LPARAM lParam) -> BOOL {
      auto& vec = *(std::vector<std::shared_ptr<OSWindowNode>>*)lParam;
      vec.push_back(
        std::static_pointer_cast<OSWindowNode>(
          std::make_shared<OSWindowWin>(hwnd)
        )
      );
      return true;
    },
    (LPARAM) &retVal
  );
  return retVal;
}

bool OSWindowWin::IsValid(void) {
  return !!IsWindow(hwnd);
}

std::wstring OSWindowWin::GetTitle(void) {
  std::wstring retVal(256, 0);
  int nch = GetWindowTextW(hwnd, &retVal[0], retVal.size());
  retVal.resize(nch);
  return retVal;
}

uint32_t OSWindowWin::GetOwningPid(void) {
  DWORD pid;
  GetWindowThreadProcessId(hwnd, &pid);
  return pid;
}

OSPoint OSWindowWin::GetPosition(void) {
  RECT rect;
  GetWindowRect(hwnd, &rect);

  OSPoint retVal;
  retVal.x = rect.left;
  retVal.y = rect.top;
  return retVal;
}

void OSWindowWin::Cloak(void) {

}

void OSWindowWin::Uncloak(void) {
}
