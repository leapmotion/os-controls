#include "stdafx.h"
#include "MakesRenderWindowFullScreen.h"

MakesRenderWindowFullScreen::MakesRenderWindowFullScreen(void) : m_isVisible(false) {
  m_rw.NotifyWhenAutowired([this]{
    m_isVisible = m_rw->IsVisible();
  });
  *this += [this] { AdjustDesktopWindow(); };
}

MakesRenderWindowFullScreen::~MakesRenderWindowFullScreen(void) {}

void MakesRenderWindowFullScreen::AdjustDesktopWindow(void) {
  if (!m_rw || !m_virtualScreen)
    return;

  auto bounds = m_virtualScreen->PrimaryScreen().Bounds();
#if _WIN32
  //On windows, if you're somehow focused & the window is the same size as the screen, Aero just stops rendering everything
  //behind you.
  ++bounds.size.width;
  ++bounds.size.height;
#endif
  m_rw->SetRect(bounds);
}
