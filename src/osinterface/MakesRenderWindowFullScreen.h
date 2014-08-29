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
  public Updatable,
  public OSVirtualScreenListener
{
public:
  MakesRenderWindowFullScreen(void);
  ~MakesRenderWindowFullScreen(void);

private:
  Autowired<OSVirtualScreen> m_virtualScreen;
  Autowired<sf::ContextSettings> m_contextSettings;
  Autowired<sf::RenderWindow> m_mw;

  // Indicates the number of desktop changes needed to be performed
  std::atomic<int> m_desktopChanged;

  /// <summary>
  /// Makes changes to the render window to track the current desktop window
  /// </summary>
  void AdjustDesktopWindow(void);

  // OSVirtualScreenListener overrides:
  void OnChange(void) override { ++m_desktopChanged; }

public:
  void Tick(std::chrono::duration<double> deltaT) override;
};

