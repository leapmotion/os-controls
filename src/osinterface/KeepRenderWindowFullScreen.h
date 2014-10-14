#pragma once
#include "OSVirtualScreen.h"
#include "RenderWindow.h"
#include "uievents/Updatable.h"

/// <summary>
/// Keep the RenderWindow in sync with the full-screen desktop of the primary screen
/// </summary>
class KeepRenderWindowFullScreen:
  public DispatchQueue,
  public Updatable,
  public OSVirtualScreenListener
{
public:
  KeepRenderWindowFullScreen(void);
  ~KeepRenderWindowFullScreen(void);

  void SetVisible(bool visible = true);
  bool IsVisible(void) const;

private:
  Autowired<OSVirtualScreen> m_virtualScreen;
  Autowired<RenderWindow> m_renderWindow;
  bool m_isVisible;

  /// <summary>
  /// Makes changes to the RenderWindow to track the current desktop window
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
