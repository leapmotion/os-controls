// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Screen.h"
#include "GLTexture2.h"

namespace leap {

void Screen::Update()
{
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
}

std::shared_ptr<GLTexture2> Screen::GetBackgroundImage() const
{
  return std::shared_ptr<GLTexture2>();
}

}
