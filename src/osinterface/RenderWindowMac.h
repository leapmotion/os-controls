#pragma once
#include "RenderWindow.h"
#include "OSVirtualScreen.h"

class RenderWindowMac :
  public RenderWindow,
  public OSVirtualScreenListener
{
public:
  RenderWindowMac(void);
  virtual ~RenderWindowMac(void);

  virtual OSPoint Postion(void) const override;
  virtual OSSize Size(void) const override;
  virtual OSRect Rect(void) const override;

  virtual void SetPosition(const OSPoint& position) override;
  virtual void SetSize(const OSSize& size) override;
  virtual void SetRect(const OSRect& rect) override;

  virtual void SetTransparent(bool transparent = true) override;
  virtual void AllowInput(bool allowInput = true) override;
  virtual void SetVisible(bool visible = true) override;

  virtual void SetActive(bool active = true) override;
  virtual void Display(void) override;

private:
  void OnScreenSizeChange(void) override;

  inline OSRect& AdjustCoordinates(OSRect& rect) const {
    rect.origin.y = m_mainDisplayHeight - (rect.origin.y + rect.size.height);
    return rect;
  }
  inline OSRect AdjustCoordinates(const OSRect& frame) const {
    OSRect rect = frame;
    return AdjustCoordinates(rect);
  }

  void* m_window; // NSWindow
  CGFloat m_mainDisplayHeight;
};
