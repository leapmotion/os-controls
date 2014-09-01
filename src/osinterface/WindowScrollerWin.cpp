#include "stdafx.h"
#include "WindowScrollerWin.h"

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerWin;
}

WindowScrollerWin::WindowScrollerWin()
{
  // Set pixels per line to the number of lines per scroll wheel notch
  UINT param = 0;
  m_pixelsPerLine.x =
    (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &param, 0) && (param != 0)) ? static_cast<float>(param) : 3;
  m_pixelsPerLine.y =
    (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &param, 0) && (param != 0)) ? static_cast<float>(param) : 3;
}

WindowScrollerWin::~WindowScrollerWin() {
  DoScrollBy(OSPointZero, OSPointZero, true); // Abruptly cancel any existing scroll
}

void WindowScrollerWin::DoScrollBy(const OSPoint& deltaPixel, const OSPoint& deltaLine, bool isMomentum) {

}
