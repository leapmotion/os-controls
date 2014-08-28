// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSScreen.h"
#include "GLTexture2.h"

void OSScreen::Update()
{
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(m_screenID, &info);
  info.rcMonitor.left;
  info.rcMonitor.top;
  info.rcMonitor.right;
  info.rcMonitor.bottom;
  m_bounds = OSRect(static_cast<float>(info.rcMonitor.left),
                    static_cast<float>(info.rcMonitor.top),
                    static_cast<float>(info.rcMonitor.right - info.rcMonitor.left),
                    static_cast<float>(info.rcMonitor.bottom - info.rcMonitor.top));
  m_isPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);
}

std::shared_ptr<GLTexture2> OSScreen::GetBackgroundImage() const
{
  return std::shared_ptr<GLTexture2>();
}
