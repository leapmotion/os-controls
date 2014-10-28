#pragma once
#include "RenderWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max

#include <memory>

class RenderContextWin;

class RenderWindowWin :
  public RenderWindow
{
public:
  RenderWindowWin(void);
  virtual ~RenderWindowWin(void);

  virtual HWND GetSystemHandle() const override;

  virtual OSPoint GetPostion(void) const override;
  virtual OSSize GetSize(void) const override;
  virtual OSRect GetRect(void) const override;

  virtual void SetPosition(const OSPoint& position) override;
  virtual void SetSize(const OSSize& size) override;
  virtual void SetRect(const OSRect& rect) override;

  virtual void SetVSync(bool vsync = true) override;
  virtual void SetTransparent(bool transparent = true) override;
  virtual void SetVisible(bool visible = true) override;
  virtual void SetCloaked(bool cloaked = true) override;

  virtual void SetActive(bool active = true) override;
  virtual void FlushBuffer(void) override;

  virtual void ProcessEvents(void) override;

private:
  std::unique_ptr<RenderContextWin> m_renderContext;

  int Create(HWND hWnd);

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
