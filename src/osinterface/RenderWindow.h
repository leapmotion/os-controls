#pragma once
#include "OSGeometry.h"

#include <autowiring/autowiring.h>

class RenderWindow :
  public ContextMember
{
public:
  RenderWindow(void) : m_isTransparent(false), m_allowInput(true) {}
  virtual ~RenderWindow(void) {}

  RenderWindow(const RenderWindow&) = delete;
  RenderWindow& operator=(const RenderWindow&) = delete;

  static RenderWindow* New(void);

  bool IsTransparent() const { return m_isTransparent; }

  virtual OSPoint Postion() const = 0;
  virtual OSSize Size() const = 0;
  virtual OSRect Rect() const = 0;

  virtual void SetPosition(const OSPoint& position) = 0;
  virtual void SetSize(const OSSize& size) = 0;
  virtual void SetRect(const OSRect& rect) = 0;

  virtual void SetTransparent(bool transparent = true) = 0;
  virtual void AllowInput(bool allowInput = true) = 0;
  virtual void SetVisible(bool visible = true) = 0;

  virtual void SetActive(bool active = true) = 0;
  virtual void Display() = 0;

protected:
  bool m_isTransparent;
  bool m_allowInput;
};
