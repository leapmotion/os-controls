#include "stdafx.h"
#include "WindowRendererWin.h"

WindowRendererWin::WindowRendererWin(void)
{
}

WindowRendererWin::~WindowRendererWin(void)
{
}

WindowRenderer* WindowRenderer::New(void) {
  return new WindowRendererWin;
}

void WindowRendererWin::Render(OSWindow& osWindow, std::vector<unsigned char>& mem) {

}
