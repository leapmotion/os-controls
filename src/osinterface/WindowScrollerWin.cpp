#include "stdafx.h"
#include "WindowScrollerWin.h"

IWindowScroller* IWindowScroller::New(void) {
  return new WindowScrollerWin;
}

WindowScrollerWin::WindowScrollerWin() :
  m_scrollPartialLine(OSPointZero)
{
  // Set pixels per line to the number of lines per scroll wheel notch
  UINT param = 0;
  m_pixelsPerLine.x =
    (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &param, 0) && (param != 0)) ? static_cast<float>(param) : 3;
  m_pixelsPerLine.y =
    (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &param, 0) && (param != 0)) ? static_cast<float>(param) : 3;
}

WindowScrollerWin::~WindowScrollerWin() {
  DoScrollBy(0.0f, 0.0f, true); // Abruptly cancel any existing scroll
}

void WindowScrollerWin::DoScrollBy(float deltaX, float deltaY, bool isMomentum) {
  const OSPoint deltaPixel = OSPointMake(deltaX*m_ppmm, deltaY*m_ppmm); // Convert to pixels
  OSPoint deltaLine  = OSPointMake(deltaPixel.x/m_pixelsPerLine.x, deltaPixel.y/m_pixelsPerLine.y); // Convert to lines

  // Adjust partial lines
  m_scrollPartialLine.x += deltaLine.x;
  m_scrollPartialLine.y += deltaLine.y;
  deltaLine.x = floor(m_scrollPartialLine.x);
  deltaLine.y = floor(m_scrollPartialLine.y);
  m_scrollPartialLine.x -= deltaLine.x;
  m_scrollPartialLine.y -= deltaLine.y;

  if (deltaLine.y != 0) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = static_cast<DWORD>(deltaLine.y*2); // Scale the step (Windows often drops values of -1 and 1)
    SendInput(1, &input, sizeof(input));
  }
  if (deltaLine.x != 0) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
    input.mi.mouseData = static_cast<DWORD>(-deltaLine.x*2); // Scale the step (see above); Also reverse direction of scroll
    SendInput(1, &input, sizeof(input));
  }
}
