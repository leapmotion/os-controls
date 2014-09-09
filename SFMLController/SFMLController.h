#pragma once

#include <SFML/Window/Window.hpp>

#include <string>

#if _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

// Encapsulates various SFML/OpenGL parameters to be passed into SFMLController::Initialize()
// Currently the following fields are supported:
// - Desired window width in pixels (ignored and reset when fullscreen = true)
// - Desired window height in pixels (ignored and reset when fullscreen = true)
// - Desired window position X in pixels (ignored and reset when fullscreen = true)
// - Desired window position Y in pixels (ignored and reset when fullscreen = true)
// - Whether to resize the window the be the resolution of the primary monitor
// - Transparency on or off
// - Whether the window will stay on top of other windows
// - Use vertical synchronization on or off
// - Use anti-aliasing on or off
// - Window title of the application
struct SFMLControllerParams {
  SFMLControllerParams() :
    windowWidth(640),
    windowHeight(480),
    windowPosX(100),
    windowPosY(100),
    fullscreen(false),
    transparentWindow(false),
    alwaysOnTop(false),
    vsync(false),
    antialias(true),
    windowTitle("GLApp")
  { }

  int windowWidth;
  int windowHeight;
  int windowPosX;
  int windowPosY;
  bool fullscreen;
  bool transparentWindow;
  bool alwaysOnTop;
  bool vsync;
  bool antialias;
  std::string windowTitle;
};

// This class bundles all the SFML usage/state into a single point of control.
class SFMLController {
public:

  SFMLController();
  ~SFMLController();

  // This creates the window, renderer, and GL context.  Throws an exception to denote error.
  void Initialize(const SFMLControllerParams& params = SFMLControllerParams());
  // Shuts everything down, in order opposite of initialization.  No exceptions should be thrown.
  void Shutdown();

  void BeginRender() const;
  void EndRender() const;

  static std::string BasePath();

  const SFMLControllerParams& GetParams() const { return m_Params; }

#if _WIN32
  HWND GetHWND() { return m_HWND; }
#endif

private:

  void InitWindow();

  void MakeTransparent();

  sf::ContextSettings m_Settings;
  mutable sf::Window m_Window;
  SFMLControllerParams m_Params;
  sf::WindowHandle m_WindowHandle;

#if _WIN32
  HWND m_HWND;
#endif
};
