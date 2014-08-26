#include "stdafx.h"
#include "OSWindowManagerInterfaceWin.h"
#include "OSWindowWin.h"

OSWindowManagerInterfaceWin::OSWindowManagerInterfaceWin(void)
{}

OSWindowManagerInterfaceWin::~OSWindowManagerInterfaceWin(void) {

}

OSWindowManagerInterface* OSWindowManagerInterface::New(void) {
  return new OSWindowManagerInterfaceWin;
}

std::vector<std::shared_ptr<OSApp>> EnumerateInteractiveApplications(void) {
  std::vector<std::shared_ptr<OSApp>> retVal;
  return retVal;
}

std::vector<std::shared_ptr<OSWindow>> OSWindowManagerInterfaceWin::EnumerateVisibleTopLevelWindows(void) {
  std::vector<std::shared_ptr<OSWindow>> retVal;
  EnumWindows(
    [](HWND hwnd, LPARAM lParam) -> BOOL {
      if(!IsWindowVisible(hwnd))
        return true;

      auto& vec = *(std::vector<std::shared_ptr<OSWindow>>*)lParam;
      vec.push_back(
        std::static_pointer_cast<OSWindow>(
          std::make_shared<OSWindowWin>(hwnd)
        )
      );
      return true;
    },
    (LPARAM) &retVal
  );
  return retVal;
}