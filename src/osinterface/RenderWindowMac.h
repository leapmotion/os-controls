#pragma once
#include "RenderWindow.h"
#include "OSVirtualScreen.h"

class RenderContextMac;

class RenderWindowMac :
  public RenderWindow,
  public OSVirtualScreenListener
{
public:
  RenderWindowMac(void);
  virtual ~RenderWindowMac(void);

  virtual OSPoint GetPostion(void) const override;
  virtual OSSize GetSize(void) const override;
  virtual OSRect GetRect(void) const override;

  virtual void SetPosition(const OSPoint& position) override;
  virtual void SetSize(const OSSize& size) override;
  virtual void SetRect(const OSRect& rect) override;

  virtual void SetVSync(bool vsync = true) override;
  virtual void SetTransparent(bool transparent = true) override;
  virtual void AllowInput(bool allowInput = true) override;
  virtual void SetVisible(bool visible = true) override;

  virtual void SetActive(bool active = true) override;
  virtual void FlushBuffer(void) override;

  virtual void ProcessEvents(void) override;

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

  std::unique_ptr<RenderContextMac> m_renderContext;
  void* m_window; // NSWindow
  CGFloat m_mainDisplayHeight;
};
