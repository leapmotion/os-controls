#include "stdafx.h"
#include "OSWindow.h"
#include "WindowRenderer.h"
#include "utility/SamplePrimitives.h"
#include <Primitives.h>
#include <GLTexture2.h>

OSWindow::OSWindow(NativeWindow::Handle hwnd) :
  hwnd(hwnd),
  m_zOrder(1)
{
}

OSWindow::~OSWindow(void)
{
}

std::shared_ptr<ImagePrimitive> OSWindow::GetWindowTexture(const std::shared_ptr<ImagePrimitive>& img) {
  AutowiredFast<WindowRenderer> wr;
  if(!wr)
    throw std::runtime_error("Cannot capture a window representation without a window renderer");

  wr->Render(*this, m_sysTexture);

  auto sz = GetSize();
  return MakePatternedTexture((size_t) sz.width, (size_t) sz.height);
}
