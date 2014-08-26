#include "stdafx.h"
#include "OSWindowManagerInterfaceWin.h"
#include "OSWindowWin.h"

OSWindowManagerInterfaceWin::OSWindowManagerInterfaceWin(void):
  m_root(nullptr)
{}

OSWindowManagerInterfaceWin::~OSWindowManagerInterfaceWin(void) {

}

OSWindowManagerInterface* OSWindowManagerInterface::New(void) {
  return new OSWindowManagerInterfaceWin;
}

std::vector<std::shared_ptr<OSWindowNode>> OSWindowManagerInterfaceWin::EnumerateTopLevel(void) {
  std::vector<std::shared_ptr<OSWindowNode>> retVal;
  EnumWindows(
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