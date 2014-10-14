#pragma once
#include "RenderContext.h"

class RenderContextWin :
  public RenderContext
{
public:
  RenderContextWin(std::shared_ptr<RenderContextWin> rootContext, HWND hWnd = nullptr);
  virtual ~RenderContextWin(void);

  void SetActive(bool active = true) override;

  void FlushBuffer(void) override;

private:
  static RenderContextWin* New(HWND hWnd);

  void* GetNativeContext(void) const override { return m_context; }

  HGLRC m_context;
  HWND m_hWnd;
  HDC m_dc;

  friend class RenderWindowWin;
};
