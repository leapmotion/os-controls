#pragma once
#include "RenderWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max

class RenderWindowWin :
  public RenderWindow
{
public:
  RenderWindowWin(bool isDoubleBuffered);
  virtual ~RenderWindowWin(void);

  virtual OSPoint GetPostion(void) const override;
  virtual OSSize GetSize(void) const override;
  virtual OSRect GetRect(void) const override;

  virtual void SetPosition(const OSPoint& position) override;
  virtual void SetSize(const OSSize& size) override;
  virtual void SetRect(const OSRect& rect) override;

  virtual void SetTransparent(bool transparent = true) override;
  virtual void AllowInput(bool allowInput = true) override;
  virtual void SetVisible(bool visible = true) override;

  virtual void SetActive(bool active = true) override;
  virtual void Display(void) override;

  virtual void ProcessEvents(void) override;

private:
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  int Create(HWND hWnd);

  HWND m_hWnd;
  HGLRC m_renderingContext;
};
