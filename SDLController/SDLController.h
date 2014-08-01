#pragma once

#include "SDL.h"
#include <SDL_syswm.h>

#include <string>

// This class bundles all the SDL usage/state into a single point of control.
struct SDLControllerParams {
  SDLControllerParams() :
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

class SDLController {
public:

  SDLController();
  ~SDLController();

  // This creates the window, renderer, and GL context.  Throws an exception to denote error.
  void Initialize(const SDLControllerParams& params = SDLControllerParams());
  // Shuts everything down, in order opposite of initialization.  No exceptions should be thrown.
  void Shutdown();

  void BeginRender() const;
  void EndRender() const;

  static std::string BasePath();

  const SDLControllerParams& GetParams() const { return m_Params; }

private:

  void InitSDL();
  void ConfigureFrameBuffer();
  void ConfigureAntialiasing();
  void InitWindow();
  void ConfigureRenderer();
  void InitGLContext();
  void ConfigureTransparentWindow();

#if _WIN32
  void MakeTransparent_Windows(const SDL_SysWMinfo &sys_wm_info);
#endif

  int m_Width;
  int m_Height;
  SDL_Window *m_SDL_Window;
  SDL_Renderer *m_SDL_Renderer;
  SDL_GLContext m_SDL_GLContext;
  SDLControllerParams m_Params;
};
