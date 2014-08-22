// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Screen.h"

namespace leap {

//
// Screen
//

Screen::Screen(const DisplayID& screenID) : m_screenID(screenID), m_isPrimary(false)
{
  Update();
}

void Screen::Update()
{
#if __APPLE__
  m_bounds = CGDisplayBounds(m_screenID);
  m_isPrimary = CGDisplayIsMain(m_screenID);
#elif _WIN32
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(m_screenID, &info);
  info.rcMonitor.left;
  info.rcMonitor.top;
  info.rcMonitor.right;
  info.rcMonitor.bottom;
  m_bounds = Rect(static_cast<Float>(info.rcMonitor.left),
                  static_cast<Float>(info.rcMonitor.top),
                  static_cast<Float>(info.rcMonitor.right - info.rcMonitor.left),
                  static_cast<Float>(info.rcMonitor.bottom - info.rcMonitor.top));
  m_isPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);
#else
  // Linux -- FIXME
#endif
}

}
