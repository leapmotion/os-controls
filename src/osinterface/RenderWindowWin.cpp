#include "stdafx.h"
#include "RenderWindowWin.h"
#include "RenderContextWin.h"

#include <GL/wglew.h>
#include <GL/gl.h>
#include <dwmapi.h>

RenderWindow* RenderWindow::New(void)
{
  return new RenderWindowWin;
}

RenderWindowWin::RenderWindowWin(void)
{
  WNDCLASS wc      = {0};
  wc.style         = CS_OWNDC;
  wc.lpfnWndProc   = WndProc;
  wc.lpszClassName = "RenderWindowWin";
  if (::RegisterClass(&wc)) {
    ::CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                    (IsTransparent() ? WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED : 0),
                     wc.lpszClassName, "", WS_VISIBLE | WS_POPUP,
                     0, 0, 0, 0, nullptr, nullptr, nullptr, this);
  }
}

RenderWindowWin::~RenderWindowWin()
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;
  if (!hWnd) {
    return;
  }
  SetVisible(false);
  ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)0);
  m_renderContext.reset();
  ::UnregisterClass("RenderWindowWin", nullptr);
}

HWND RenderWindowWin::GetSystemHandle() const
{
  return m_renderContext ? m_renderContext->m_hWnd : nullptr;
}

int RenderWindowWin::Create(HWND hWnd)
{
  ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

  m_renderContext.reset(RenderContextWin::New(hWnd));

  SetTransparent(m_isTransparent);
  AllowInput(m_allowInput);

  return 0;
}

OSPoint RenderWindowWin::GetPostion() const
{
  if (!m_renderContext) {
    return OSPointZero;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return OSPointZero;
  }
  RECT rect;

  if (!::GetWindowRect(hWnd, &rect)) {
    return OSPointZero;
  }
  return OSPointMake(static_cast<float>(rect.left), static_cast<float>(rect.top));
}

OSSize RenderWindowWin::GetSize() const
{
  if (!m_renderContext) {
    return OSSizeZero;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return OSSizeZero;
  }
  RECT rect;

  if (!::GetWindowRect(hWnd, &rect)) {
    return OSSizeZero;
  }
  return OSSizeMake(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
}

OSRect RenderWindowWin::GetRect() const
{
  if (!m_renderContext) {
    return OSRectZero;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return OSRectZero;
  }
  RECT rect;

  if (!::GetWindowRect(hWnd, &rect)) {
    return OSRectZero;
  }
  return OSRectMake(static_cast<float>(rect.left), static_cast<float>(rect.top),
                    static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
}

void RenderWindowWin::SetPosition(const OSPoint& position)
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return;
  }
  ::SetWindowPos(hWnd, HWND_TOPMOST, static_cast<int>(position.x), static_cast<int>(position.y), 0, 0, SWP_NOSIZE);
}

void RenderWindowWin::SetSize(const OSSize& size)
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return;
  }
  ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, static_cast<int>(size.width), static_cast<int>(size.height), SWP_NOMOVE);
}

void RenderWindowWin::SetRect(const OSRect& rect)
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return;
  }
  ::SetWindowPos(hWnd, HWND_TOPMOST, static_cast<int>(rect.origin.x), static_cast<int>(rect.origin.y),
                                     static_cast<int>(rect.size.width), static_cast<int>(rect.size.height), 0);
}

void RenderWindowWin::SetVSync(bool vsync)
{
  if (!m_renderContext) {
    return;
  }
  m_useVSync = vsync;

  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
      reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
  if (wglSwapIntervalEXT) {
    wglSwapIntervalEXT(m_useVSync ? 1 : 0);
  }
}

void RenderWindowWin::SetTransparent(bool transparent)
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return;
  }
  m_isTransparent = transparent;

  LONG flags = ::GetWindowLongA(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
  ::SetWindowLongA(hWnd, GWL_EXSTYLE, flags);
  ::SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);

  DWM_BLURBEHIND bb = { 0 };
  bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
  bb.fEnable = true;
  bb.hRgnBlur = CreateRectRgn(0, 0, 1, 1);
  ::DwmEnableBlurBehindWindow(hWnd, &bb);
}

void RenderWindowWin::AllowInput(bool allowInput)
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return;
  }
  m_allowInput = allowInput;
  const LONG prevStyle = ::GetWindowLongA(hWnd, GWL_EXSTYLE);
  const LONG modStyle = WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
  const LONG style = m_allowInput ? (prevStyle & ~modStyle) : (prevStyle | modStyle);
  ::SetWindowLongA(hWnd, GWL_EXSTYLE, style);
}

void RenderWindowWin::SetVisible(bool visible)
{
  if (!m_renderContext) {
    return;
  }
  HWND hWnd = m_renderContext->m_hWnd;

  if (!hWnd) {
    return;
  }
  m_isVisible = visible;
  ::ShowWindow(hWnd, visible ? SW_SHOW : SW_HIDE);
}

void RenderWindowWin::SetActive(bool active)
{
  if (!m_renderContext) {
    return;
  }
  m_renderContext->SetActive(active);
}

void RenderWindowWin::FlushBuffer(void)
{
  if (!m_renderContext) {
    return;
  }
  m_renderContext->FlushBuffer();
}

void RenderWindowWin::ProcessEvents(void)
{
  MSG message;

  while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
    ::TranslateMessage(&message);
    ::DispatchMessageW(&message);
  }
}

LRESULT CALLBACK RenderWindowWin::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message) {
  case WM_CREATE:
    {
      CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
      if (create) {
        RenderWindowWin* that = reinterpret_cast<RenderWindowWin*>(create->lpCreateParams);
        if (that) {
          return that->Create(hWnd);
        }
      }
    }
    return -1;
  case WM_ERASEBKGND:
  case WM_NCPAINT:
    break;
  default:
    return ::DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
