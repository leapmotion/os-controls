// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "VirtualScreen.h"

namespace leap  {

class VirtualScreenWin :
  public VirtualScreen
{
  public:
    VirtualScreenWin();
    virtual ~VirtualScreenWin();

  protected:
    virtual std::vector<Screen> GetScreens() const override;

  private:
    friend class VirtualScreenHelperClass;

    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK EnumerateDisplays(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

    HWND m_hWnd;
};

}
