#include "stdafx.h"
#include "MakesRenderWindowFullScreen.h"
#include "utility/NativeWindow.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

MakesRenderWindowFullScreen::MakesRenderWindowFullScreen(void) {
  *this += [this] { AdjustDesktopWindow(); };
}
MakesRenderWindowFullScreen::~MakesRenderWindowFullScreen(void) {}

void MakesRenderWindowFullScreen::AdjustDesktopWindow(void) {
  if(!m_mw || !m_virtualScreen || !m_contextSettings)
    return;

  const sf::Vector2i olPosition = m_mw->getPosition();
  const sf::Vector2u oldSize = m_mw->getSize();

  const auto bounds = m_virtualScreen->PrimaryScreen().Bounds();
  const sf::Vector2i newPosition{static_cast<int32_t>(bounds.origin.x), static_cast<int32_t>(bounds.origin.y)};
  const sf::Vector2u newSize{static_cast<uint32_t>(bounds.size.width), static_cast<uint32_t>(bounds.size.height)};

  if(oldSize != newSize) {
    m_mw->create(sf::VideoMode(newSize.x, newSize.y), "Leap Hand Control", sf::Style::None, *m_contextSettings);
  }
  m_mw->setVisible(false);
#if __APPLE__
  // The SFML code for positioning a window on Mac OS X incorrectly uses the
  // height of the screen containing the window to flip the Y coordinates. It
  // should always use the height of the default screen. To workaround this
  // shortcoming, first set position of this window to the origin. It should
  // hopefully be on the default screen, but if the height differences between
  // screens is too far off, it could potentially miss.
  m_mw->setPosition(sf::Vector2i{0,0});
#endif
  m_mw->setPosition(newPosition);
  const auto handle = m_mw->getSystemHandle();
  NativeWindow::MakeTransparent(handle);
  NativeWindow::MakeAlwaysOnTop(handle);
  NativeWindow::AllowInput(handle, false);
  m_mw->setVisible(true);
  NativeWindow::AbandonFocus(m_mw->getSystemHandle());
}
