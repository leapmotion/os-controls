#include "stdafx.h"
#include "ExposeViewWindow.h"
#include "osinterface/OSWindow.h"
#include "graphics/RenderFrame.h"

ExposeViewWindow::ExposeViewWindow(OSWindow& osWindow):
  m_osWindow(osWindow.shared_from_this()),
  m_texture(nullptr)
{

}

ExposeViewWindow::~ExposeViewWindow(void) {}

void ExposeViewWindow::UpdateTexture(void) {
  m_osWindow->GetWindowTexture(m_texture);
}

void ExposeViewWindow::Render(const RenderFrame& frame) const {
  m_texture.Draw(frame.renderState);
}