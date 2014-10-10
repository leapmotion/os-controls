/*==================================================================================================================

    Copyright (c) 2010 - 2013 Leap Motion. All rights reserved.

  The intellectual and technical concepts contained herein are proprietary and confidential to Leap Motion, and are
  protected by trade secret or copyright law. Dissemination of this information or reproduction of this material is
  strictly forbidden unless prior written permission is obtained from Leap Motion.

===================================================================================================================*/
#include "stdafx.h"
#include "LPWindowWin.h"

#include <GL/gl.h>
#include <dwmapi.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "dwmapi.lib")

LPWindowWin::LPWindowWin(bool isTransparent, bool isDoubleBuffered) :
  LPWindowBase(isTransparent, isDoubleBuffered), m_hWnd(nullptr)
{
  WNDCLASS wc      = {0};
  wc.style         = CS_OWNDC;
  wc.lpfnWndProc   = WndProc;
  wc.lpszClassName = "LPWindowWin";
  if (::RegisterClass(&wc)) {
    ::CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                    (IsTransparent() ? WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED : 0),
                     wc.lpszClassName, "", WS_VISIBLE | WS_POPUP,
                     0, 0, 0, 0, nullptr, nullptr, nullptr, this);
  }
}

LPWindowWin::~LPWindowWin()
{
  ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)0);
  Hide();
  if (m_renderingContext) {
    ::wglMakeCurrent(nullptr, nullptr);
    ::wglDeleteContext(m_renderingContext);
    m_renderingContext = nullptr;
  }
  ::DestroyWindow(m_hWnd);
  ::UnregisterClass("LPWindowWin", nullptr);
}

int LPWindowWin::Create(HWND hWnd)
{
  ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
   (IsDoubleBuffered() ?
    PFD_DOUBLEBUFFER : 0) |
    PFD_SUPPORT_OPENGL |
    PFD_DRAW_TO_WINDOW,       // Flags
    PFD_TYPE_RGBA,            // The kind of framebuffer. RGBA or palette.
    24,                       // Color depth of the framebuffer
    8, 0, 8, 0, 8, 0, 8, 0,
    0, 0, 0, 0, 0,
    0,                        // Number of bits for the depthbuffer
    0,                        // Number of bits for the stencilbuffer
    0,                        // Number of Aux buffers in the framebuffer.
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
  };

  const HDC deviceContext = ::GetDC(hWnd);
  const int pixelFormatIndex = ::ChoosePixelFormat(deviceContext, &pfd);
  ::SetPixelFormat(deviceContext, pixelFormatIndex, &pfd);

  m_renderingContext = ::wglCreateContext(deviceContext);
  ::wglMakeCurrent(deviceContext, m_renderingContext);

  if (IsTransparent()) {
    ::SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
  }
  m_hWnd = hWnd;

  return 0;
}

LPPoint LPWindowWin::Postion() const
{
  RECT rect;

  if (m_hWnd && ::GetWindowRect(m_hWnd, &rect)) {
    return LPPointMake(static_cast<LPFloat>(rect.left), static_cast<LPFloat>(rect.top));
  }
  return LPPointZero;
}

LPSize LPWindowWin::Size() const
{
  RECT rect;

  if (m_hWnd && ::GetWindowRect(m_hWnd, &rect)) {
    return LPSizeMake(static_cast<LPFloat>(rect.right - rect.left), static_cast<LPFloat>(rect.bottom - rect.top));
  }
  return LPSizeZero;
}

LPRect LPWindowWin::Rect() const
{
  RECT rect;

  if (m_hWnd && ::GetWindowRect(m_hWnd, &rect)) {
    return LPRectMake(static_cast<LPFloat>(rect.left), static_cast<LPFloat>(rect.top),
                      static_cast<LPFloat>(rect.right - rect.left), static_cast<LPFloat>(rect.bottom - rect.top));
  }
  return LPRectZero;
}

void LPWindowWin::SetPosition(const LPPoint& position)
{
  if (m_hWnd) {
    ::SetWindowPos(m_hWnd, HWND_TOPMOST, static_cast<int>(position.x), static_cast<int>(position.y), 0, 0, SWP_NOSIZE);
  }
}

void LPWindowWin::SetSize(const LPSize& size)
{
  if (m_hWnd) {
    ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, static_cast<int>(size.width), static_cast<int>(size.height), SWP_NOMOVE);
  }
}

void LPWindowWin::SetRect(const LPRect& rect)
{
  if (m_hWnd) {
    ::SetWindowPos(m_hWnd, HWND_TOPMOST, static_cast<int>(rect.origin.x), static_cast<int>(rect.origin.y),
                                         static_cast<int>(rect.size.width), static_cast<int>(rect.size.height), 0);
  }
}

void LPWindowWin::Show(bool show)
{
  if (m_hWnd) {
    ::ShowWindow(m_hWnd, show ? SW_SHOW : SW_HIDE);
  }
}

void LPWindowWin::Hide()
{
  Show(false);
}

void LPWindowWin::Redraw()
{
  if (m_hWnd) {
    ::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INTERNALPAINT);
  }
}

LRESULT CALLBACK LPWindowWin::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message) {
  case WM_CREATE:
    {
      CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
      if (create) {
        LPWindowWin* that = reinterpret_cast<LPWindowWin*>(create->lpCreateParams);
        if (that) {
          return that->Create(hWnd);
        }
      }
    }
    return -1;
  case WM_ERASEBKGND:
  case WM_NCPAINT:
    break;
  case WM_SIZE:
    {
      LPWindowWin* that = reinterpret_cast<LPWindowWin*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
      if (that) {
        LPFloat width =  static_cast<LPFloat>( lParam        & 0xFFFF);
        LPFloat height = static_cast<LPFloat>((lParam >> 16) & 0xFFFF);

        that->OnSize(LPSizeMake(width, height));
      }
    }
    break;
  case WM_PAINT:
    {
      LPWindowWin* that = reinterpret_cast<LPWindowWin*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
      if (that && that->m_renderingContext) {
        ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // Make sure that we are on top
        PAINTSTRUCT ps;
        ::BeginPaint(hWnd, &ps);
        if (that->IsTransparent()) {
          DWM_BLURBEHIND bb = {0};
          bb.dwFlags = DWM_BB_ENABLE;
          bb.fEnable = true;
          bb.hRgnBlur = nullptr;
          ::DwmEnableBlurBehindWindow(hWnd, &bb);
        }
        that->OnRender();
        if (that->IsDoubleBuffered()) {
          ::SwapBuffers(GetDC(hWnd));
        } else {
          ::glFlush();
        }
        ::EndPaint(hWnd, &ps);
      } else {
        return ::DefWindowProc(hWnd, message, wParam, lParam);
      }
    }
    break;
  default:
    return ::DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
