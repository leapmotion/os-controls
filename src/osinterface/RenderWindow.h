#pragma once
#include "OSGeometry.h"

#include <autowiring/autowiring.h>

class RenderContext;

class RenderWindow :
  public ContextMember
{
public:
  RenderWindow(void):
    m_useVSync(true), m_isTransparent(true), m_isVisible(false), m_allowInput(true) {}
  virtual ~RenderWindow(void) {}

  RenderWindow(const RenderWindow&) = delete;
  RenderWindow& operator=(const RenderWindow&) = delete;

  static RenderWindow* New(void);

  bool UseVSync(void) const { return m_useVSync; }
  bool IsTransparent(void) const { return m_isTransparent; }
  bool IsVisible(void) const { return m_isVisible; }
  bool DoesAllowInput(void) const {return m_allowInput; }

  virtual OSPoint GetPostion(void) const = 0;
  virtual OSSize GetSize(void) const = 0;
  virtual OSRect GetRect(void) const = 0;

  virtual void SetPosition(const OSPoint& position) = 0;
  virtual void SetSize(const OSSize& size) = 0;
  virtual void SetRect(const OSRect& rect) = 0;

  virtual void SetVSync(bool vsync = true) = 0;
  virtual void SetTransparent(bool transparent = true) = 0;
  virtual void SetVisible(bool visible = true) = 0;
  virtual void AllowInput(bool allowInput = true) = 0;

  virtual void SetActive(bool active = true) = 0;
  virtual void FlushBuffer(void) = 0;

  virtual void ProcessEvents() = 0;

protected:
  bool m_useVSync;
  bool m_isTransparent;
  bool m_isVisible;
  bool m_allowInput;
};
