#include "stdafx.h"
#include "MakesRenderWindowFullScreen.h"
#include "utility/NativeWindow.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

MakesRenderWindowFullScreen::MakesRenderWindowFullScreen(void) :
  m_desktopChanged{1}
{
  m_mw.NotifyWhenAutowired([this] {
    m_mw->setFramerateLimit(0);
    m_mw->setVerticalSyncEnabled(true);
  });
}

MakesRenderWindowFullScreen::~MakesRenderWindowFullScreen(void)
{
}

void MakesRenderWindowFullScreen::AdjustDesktopWindow(void) {
  if(!m_mw || !m_virtualScreen || !m_contextSettings)
    return;

  m_mw->setVisible(false);
  const sf::Vector2i olPosition = m_mw->getPosition();
  const sf::Vector2u oldSize = m_mw->getSize();

  const auto bounds = m_virtualScreen->PrimaryScreen().Bounds();
  const sf::Vector2i newPosition = {static_cast<int32_t>(bounds.origin.x),
    static_cast<int32_t>(bounds.origin.y)};
  const sf::Vector2u newSize = {static_cast<uint32_t>(bounds.size.width),
    static_cast<uint32_t>(bounds.size.height)};

  if(oldSize != newSize) {
    m_mw->create(sf::VideoMode(newSize.x, newSize.y), "Leap Os Control", sf::Style::None, *m_contextSettings);
  }
  m_mw->setPosition(newPosition);
  const auto handle = m_mw->getSystemHandle();
  NativeWindow::MakeTransparent(handle);
  NativeWindow::MakeAlwaysOnTop(handle);
  NativeWindow::AllowInput(handle, false);
  m_mw->setVisible(true);
}

void MakesRenderWindowFullScreen::Tick(std::chrono::duration<double> deltaT) {
  if(!m_desktopChanged)
    return;

  // Our chance to position and possibly recreate the window if the desktop has changed
  --m_desktopChanged;
  AdjustDesktopWindow();
}