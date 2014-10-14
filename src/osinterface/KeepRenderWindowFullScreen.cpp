#include "stdafx.h"
#include "KeepRenderWindowFullScreen.h"

KeepRenderWindowFullScreen::KeepRenderWindowFullScreen(void) : m_isVisible(false) {
  m_renderWindow.NotifyWhenAutowired([this]{
    m_isVisible = m_renderWindow->IsVisible();
  });
  *this += [this] { AdjustDesktopWindow(); };
}

KeepRenderWindowFullScreen::~KeepRenderWindowFullScreen(void) {}

void KeepRenderWindowFullScreen::AdjustDesktopWindow(void) {
  if (!m_renderWindow || !m_virtualScreen)
    return;

  auto bounds = m_virtualScreen->PrimaryScreen().Bounds();
#if _WIN32
  //On windows, if you're somehow focused & the window is the same size as the screen, Aero just stops rendering everything
  //behind you.
  ++bounds.size.width;
  ++bounds.size.height;
#endif
  m_renderWindow->SetRect(bounds);
}
