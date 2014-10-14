#pragma once
#include "RenderContext.h"

class RenderContextMac :
  public RenderContext
{
public:
  RenderContextMac(std::shared_ptr<RenderContextMac> rootContext);
  virtual ~RenderContextMac(void);

  void SetActive(bool active = true) override;

  void FlushBuffer(void) override;

private:
  void* GetNativeContext(void) const override { return m_context; }

  void* m_context;
  void* m_rootWindow;

  friend class RenderWindowMac;
};
