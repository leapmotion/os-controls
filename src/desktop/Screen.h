// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "ScreenBase.h"

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

class GLTexture2;

namespace leap {

#if __APPLE__
using DisplayID = CGDirectDisplayID;
#elif _WIN32
using DisplayID = HMONITOR;
#else
using DisplayID = unsigned long;
#endif

class Screen : public ScreenBase {
  public:
    virtual ~Screen() {}

    DisplayID ID() const { return m_screenID; }
    bool IsPrimary() const { return m_isPrimary; }

    std::shared_ptr<GLTexture2> GetBackgroundImage() const;

  private:
    Screen(const DisplayID& screenID) : m_screenID(screenID), m_isPrimary(false) { Update(); }

    void Update();

    DisplayID m_screenID;
    bool m_isPrimary;

    friend class VirtualScreenMac;
    friend class VirtualScreenWin;
};

}
