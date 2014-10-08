#pragma once
#include "osinterface/OSVirtualScreen.h"
#include "uievents/Updatable.h"

namespace sf {
  class RenderWindow;
  struct ContextSettings;
}

/// <summary>
/// Implements an OpenGL window that attempts to track changes to the full-screen desktop
/// </summary>
class MakesRenderWindowFullScreen:
  public DispatchQueue,
  public Updatable,
  public OSVirtualScreenListener
{
public:
  MakesRenderWindowFullScreen(void);
  ~MakesRenderWindowFullScreen(void);

  void SetVisible(bool visible = true);
  bool IsVisible(void) const { return m_isVisible; }

private:
  Autowired<OSVirtualScreen> m_virtualScreen;
  Autowired<sf::ContextSettings> m_contextSettings;
  Autowired<sf::RenderWindow> m_mw;
  bool m_isVisible;

  /// <summary>
  /// Makes changes to the render window to track the current desktop window
  /// </summary>
  void AdjustDesktopWindow(void);

  // OSVirtualScreenListener overrides:
  void OnScreenSizeChange(void) override {
    *this += [this] { AdjustDesktopWindow(); };
  }

public:
  void Tick(std::chrono::duration<double> deltaT) override {
    DispatchAllEvents();
  }
};
