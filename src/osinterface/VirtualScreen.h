// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "Screen.h"
#include <vector>
#include <mutex>

namespace leap  {

class VirtualScreenListener {
  public:
    VirtualScreenListener(void) {}
    virtual ~VirtualScreenListener(void) {}

    virtual void OnChange(void) = 0;
};

class VirtualScreen :
  public ContextMember,
  public ScreenBase
{
  public:
    VirtualScreen();
    virtual ~VirtualScreen();

    static VirtualScreen* New(void);

    std::vector<Screen> Screens() const {
      std::lock_guard<std::mutex> lock(m_mutex);
      return m_screens;
    }

    Screen PrimaryScreen() const;
    Screen ClosestScreen(const Point& position) const;

  protected:
    virtual std::vector<Screen> GetScreens() const = 0;

    void Update();

  private:
    Rect ComputeBounds(const std::vector<Screen>& screens);
    Point ClipPosition(const Point& position, uint32_t* index) const;

    mutable std::mutex m_mutex;
    std::vector<Screen> m_screens;
};

}
