#pragma once
#include "OSGeometry.h"

#include <autowiring/autowiring.h>

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#endif

class RenderContext;

class RenderWindow :
  public ContextMember
{
public:
  RenderWindow(void):
    m_useVSync(true), m_isTransparent(false), m_isVisible(false), m_isCloaked(false) {}
  virtual ~RenderWindow(void) {}

  RenderWindow(const RenderWindow&) = delete;
  RenderWindow& operator=(const RenderWindow&) = delete;

  static RenderWindow* New(void);

  bool UseVSync(void) const { return m_useVSync; }
  bool IsTransparent(void) const { return m_isTransparent; }
  bool IsVisible(void) const { return m_isVisible; }

#if _WIN32
  using WindowHandle = HWND;
#elif __APPLE__
  using WindowHandle = void*; // NSWindow
#endif

  virtual WindowHandle GetSystemHandle() const = 0;

  virtual OSPoint GetPostion(void) const = 0;
  virtual OSSize GetSize(void) const = 0;
  virtual OSRect GetRect(void) const = 0;

  virtual void SetPosition(const OSPoint& position) = 0;
  virtual void SetSize(const OSSize& size) = 0;
  virtual void SetRect(const OSRect& rect) = 0;

  virtual void SetVSync(bool vsync = true) = 0;
  virtual void SetTransparent(bool transparent = true) = 0;
  virtual void SetVisible(bool visible = true) = 0;
  virtual void SetCloaked(bool cloaked = true) = 0;

  //if called with true, and then false, it must attempt to restore the previous focus
  virtual void SetKBFocus(bool focus = true) = 0;

  virtual void SetActive(bool active = true) = 0;
  virtual void FlushBuffer(void) = 0;

  virtual void ProcessEvents() = 0;

protected:
  bool m_useVSync;
  bool m_isTransparent;
  bool m_isVisible;
  bool m_isCloaked;
};
