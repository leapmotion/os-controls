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
  m_linesPerClick.x =
    (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &param, 0) && (param != 0)) ? static_cast<float>(param) : 3;
  m_linesPerClick.y =
    (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &param, 0) && (param != 0)) ? static_cast<float>(param) : 3;

  m_pixelsPerLine.x = 12;
  m_pixelsPerLine.y = 12;
}

WindowScrollerWin::~WindowScrollerWin() {
  DoScrollBy(0.0f, 0.0f, true); // Abruptly cancel any existing scroll
}

void WindowScrollerWin::DoScrollBy(float deltaX, float deltaY, bool isMomentum) {
  // Convert to lines
  OSPoint wheelDelta{
    WHEEL_DELTA * (deltaX / m_linesPerClick.x) / m_pixelsPerLine.x,
    WHEEL_DELTA * (deltaY / m_linesPerClick.y) / m_pixelsPerLine.y
  };

  // Adjust partial lines
  m_scrollPartialLine.x += wheelDelta.x;
  m_scrollPartialLine.y += wheelDelta.y;
  wheelDelta.x = floor(m_scrollPartialLine.x);
  wheelDelta.y = floor(m_scrollPartialLine.y);
  m_scrollPartialLine.x -= wheelDelta.x;
  m_scrollPartialLine.y -= wheelDelta.y;

  if (wheelDelta.y != 0) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = static_cast<DWORD>(wheelDelta.y);
    SendInput(1, &input, sizeof(input));
  }

  if (wheelDelta.x != 0) {
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
    input.mi.mouseData = static_cast<DWORD>(-wheelDelta.x);
    SendInput(1, &input, sizeof(input));
  }
}
