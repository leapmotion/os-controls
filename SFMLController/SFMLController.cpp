#include "SFMLController.h"

#if _WIN32
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

#include <cassert>
#include <iostream> // TEMP
#include <stdexcept>

SFMLController::SFMLController() { }

SFMLController::~SFMLController() {
  assert(m_SDL_Window == nullptr && "did not shut down properly!");
  assert(m_SDL_Renderer == nullptr && "did not shut down properly!");
  assert(m_SDL_GLContext == nullptr && "did not shut down properly!");
}

void SFMLController::Initialize(const SFMLControllerParams& params) {
  m_Params = params;

  ConfigureFrameBuffer();
  ConfigureAntialiasing();
  InitWindow();
  ConfigureRenderer();
  InitGLContext();
  ConfigureTransparentWindow();
}

void SFMLController::Shutdown () {
  // TODO: shutdown anything else necessary, video, and SDL (in reverse order as init)
  //if (m_SDL_GLContext != nullptr) {
  //  SDL_GL_DeleteContext(m_SDL_GLContext);
  //  m_SDL_GLContext = nullptr;
  //}
  //if (m_SDL_Renderer != nullptr) {
  //  SDL_DestroyRenderer(m_SDL_Renderer);
  //  m_SDL_Renderer = nullptr;
  //}
  //if (m_SDL_Window != nullptr) {
  //  SDL_DestroyWindow(m_SDL_Window);
  //  m_SDL_Window = nullptr;
  //}
  //SDL_Quit();
}

void SFMLController::BeginRender () const {
  // Do whatever needs to be done before rendering a frame.
}

void SFMLController::EndRender () const {
  //SDL_GL_SwapWindow(m_SDL_Window);
}

std::string SFMLController::BasePath () {
#if 0
  return std::string(SDL_GetBasePath());
#endif
  return "";
}

// It's necessary to put the Apple-specific code in a separate file because
// it is Objective-C++ and needs to be compiled as such (the source file name
// is MakeTransparent_Apple.mm).
#if __APPLE__
extern void MakeTransparent_Apple (const SDL_SysWMinfo &sys_wm_info, SDL_GLContext c);
#endif

#if defined(WIN32)
//void SFMLController::MakeTransparent_Windows (const SDL_SysWMinfo &sys_wm_info) {
//  HWND hWnd = sys_wm_info.info.win.window;
//  if (hWnd) {
//    LONG flags = ::GetWindowLongA(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT;
//    if (m_Params.alwaysOnTop) {
//      flags |= WS_EX_TOPMOST;
//    }
//    ::SetWindowLongA(hWnd, GWL_EXSTYLE, flags);
//    ::SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
//  } else {
//    throw std::runtime_error("Error retrieiving native window");
//  }
//  if (m_Params.alwaysOnTop) {
//    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//  }
//  DWM_BLURBEHIND bb = { 0 };
//  bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
//  bb.fEnable = true;
//  bb.hRgnBlur = CreateRectRgn(0, 0, 1, 1);
//  ::DwmEnableBlurBehindWindow(hWnd, &bb);
//}
#endif

void SFMLController::ConfigureFrameBuffer() {
#if 0
  // Set some SDL attributes. Must be done before creating GL context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  if (m_Params.transparentWindow) {
    // Must allocate an alpha channel when using transparent window
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  }
#endif
}

void SFMLController::ConfigureAntialiasing() {
#if 0
  if (m_Params.antialias) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
  }
#endif
}

void SFMLController::InitWindow() {
#if 0
  if (m_Params.fullscreen) {
    SDL_Rect rect;
    if (SDL_GetDisplayBounds(0, &rect) == 0) {
      m_Params.windowPosX = 0;
      m_Params.windowPosY = 0;
      m_Params.windowWidth = rect.w;
      m_Params.windowHeight = rect.h-1;
    }
  }

  Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
  if (m_Params.transparentWindow) {
    // Transparent window only works properly when the window is borderless
    windowFlags |= SDL_WINDOW_BORDERLESS;
  }

  // Create a window.
  m_SDL_Window = SDL_CreateWindow(m_Params.windowTitle.c_str(), m_Params.windowPosX, m_Params.windowPosY, m_Params.windowWidth, m_Params.windowHeight, windowFlags);

  if (m_SDL_Window == nullptr) {
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
  }
#endif
  m_Window.create(sf::VideoMode(640, 480, 32U), "OpenGL", sf::Style::Default, m_Settings);
}

void SFMLController::ConfigureRenderer() {
#if 0
  // Create a renderer.  NOTE: it may be platform-specific as to if we want V-sync.
  Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
  if (m_Params.vsync) {
    rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
  }

  m_SDL_Renderer = SDL_CreateRenderer(m_SDL_Window, -1, rendererFlags);
  if (m_SDL_Renderer == nullptr) {
    SDL_DestroyWindow(m_SDL_Window);
    m_SDL_Window = nullptr;
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
  }
#endif
}

void SFMLController::InitGLContext() {
#if 0
  // Create a GL context.
  m_SDL_GLContext = SDL_GL_CreateContext(m_SDL_Window);
  if (m_SDL_GLContext == nullptr) {
    SDL_DestroyRenderer(m_SDL_Renderer);
    m_SDL_Renderer = nullptr;
    SDL_DestroyWindow(m_SDL_Window);
    m_SDL_Window = nullptr;
    SDL_Quit();
    throw std::runtime_error(SDL_GetError());
  }
#endif
}

void SFMLController::ConfigureTransparentWindow() {
#if 0
  if (m_Params.transparentWindow) {
    struct SDL_SysWMinfo sys_wm_info;
    SDL_VERSION(&sys_wm_info.version);

    // Retrieve the window info.
    if (!SDL_GetWindowWMInfo(m_SDL_Window, &sys_wm_info)) {
      throw std::runtime_error("Error retrieving window WM info");
    }

    // Make the OS-specific call to make the window transparent.
    switch (sys_wm_info.subsystem) {
#ifdef WIN32 
    case SDL_SYSWM_WINDOWS:
      MakeTransparent_Windows(sys_wm_info);
      break;
#elif __APPLE__ 
    case SDL_SYSWM_COCOA:
      MakeTransparent_Apple(sys_wm_info, m_SDL_GLContext);
      break;
#else 
    case SDL_SYSWM_X11:
      // TODO
      break;
#endif 
    default:
      throw std::runtime_error("Error identifying WM subsystem");
      break;
    }
  }
#endif
}
