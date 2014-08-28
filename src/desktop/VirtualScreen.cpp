// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "VirtualScreen.h"

namespace leap {

//
// VirtualScreen
//

VirtualScreen::VirtualScreen() : ContextMember("VirtualScreen")
{
}

VirtualScreen::~VirtualScreen()
{
}

Screen VirtualScreen::PrimaryScreen() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  uint32_t numDisplays = static_cast<uint32_t>(m_screens.size());
  if (numDisplays > 1) {
    for (uint32_t i = 0; i < numDisplays; i++) {
      if (m_screens[i].IsPrimary()) {
        return m_screens[i];
      }
    }
  }
  if (m_screens.empty()) {
    throw std::runtime_error("Unable to detect screens");
  }
  return m_screens[0];
}

Screen VirtualScreen::ClosestScreen(const Point& position) const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  uint32_t numDisplays = static_cast<uint32_t>(m_screens.size());

  if (numDisplays > 1) {
    for (uint32_t i = 0; i < numDisplays; i++) {
      if (RectContainsPoint(m_screens[i].Bounds(), position)) {
        return m_screens[i];
      }
    }
    int bestIndex = 0;
    Float bestSquaredDistance = 0;
    for (uint32_t i = 0; i < numDisplays; i++) {
      Point clipped = m_screens[i].ClipPosition(position);
      const Float dx = (clipped.x - position.x);
      const Float dy = (clipped.y - position.y);
      Float squaredDistance = dx*dx + dy*dy;
      if (i == 0 || squaredDistance < bestSquaredDistance) {
        bestIndex = i;
        bestSquaredDistance = squaredDistance;
      }
    }
    return m_screens[bestIndex];
  }
  if (m_screens.empty()) {
    throw std::runtime_error("Unable to detect screens");
  }
  return m_screens[0];
}

void VirtualScreen::Update()
{
  auto screens = GetScreens();
  std::unique_lock<std::mutex> lock(m_mutex);
  m_screens = screens;
  m_bounds = ComputeBounds(m_screens);
  lock.unlock();
  AutoFired<VirtualScreenListener> vsl;
  vsl(&VirtualScreenListener::OnChange)();
}

Rect VirtualScreen::ComputeBounds(const std::vector<Screen>& screens)
{
  size_t numScreens = screens.size();

  if (numScreens == 1) {
    return screens[0].Bounds();
  } else if (numScreens > 1) {
    Rect bounds = screens[0].Bounds();

    for (size_t i = 1; i < numScreens; i++) {
      bounds = RectUnion(bounds, screens[i].Bounds());
    }
    return bounds;
  } else {
    return RectZero;
  }
}

}
