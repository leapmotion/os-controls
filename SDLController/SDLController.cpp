#include "SDLController.h"

#if _WIN32
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

#include <cassert>
#include <stdexcept>

#include "gl_glext_glu.h"

SDLController::SDLController ()
  :
  m_SDL_Window(nullptr),
  m_SDL_Renderer(nullptr),
  m_SDL_GLContext(nullptr)
{
#if _WIN32
  m_HWND = nullptr;
#endif
}

SDLController::~SDLController () {
  assert(m_SDL_Window == nullptr && "did not shut down properly!");
  assert(m_SDL_Renderer == nullptr && "did not shut down properly!");
  assert(m_SDL_GLContext == nullptr && "did not shut down properly!");
}

void SDLController::Initialize(const SDLControllerParams& params) {
  m_Params = params;

  InitSDL();
  ConfigureFrameBuffer();
  ConfigureAntialiasing();
  InitWindow();
  ConfigureRenderer();
  InitGLContext();
  InitGlew();
  ConfigureTransparentWindow();
}

void SDLController::Shutdown () {
  CleanUpInitializedResources();
}

void SDLController::BeginRender () const {
  // Make this the current GL context.  SDL_GL_MakeCurrent() returns 0 upon success
  if (SDL_GL_MakeCurrent(m_SDL_Window, m_SDL_GLContext) != 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLController::EndRender () const {
  SDL_GL_SwapWindow(m_SDL_Window);
}

std::string SDLController::BasePath () {
  static std::string basePath;
  if (basePath.empty()) {
    char * sdlBasePath = SDL_GetBasePath();
    if (sdlBasePath) {
      basePath = std::string(sdlBasePath);
      SDL_free(sdlBasePath);
    } else {
      throw std::runtime_error("Could not get the SDL Base Path");
    }
  }
  
  return basePath;
}

void SDLController::ToggleFullscreen() {
  m_Params.fullscreen = !m_Params.fullscreen;
  SDL_SetWindowFullscreen(m_SDL_Window, m_Params.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  SDL_GetWindowSize(m_SDL_Window, &m_Params.windowWidth, &m_Params.windowHeight);
}

// It's necessary to put the Apple-specific code in a separate file because
// it is Objective-C++ and needs to be compiled as such (the source file name
// is MakeTransparent_Apple.mm).
#if __APPLE__
extern void MakeTransparent_Apple (const SDL_SysWMinfo &sys_wm_info, SDL_GLContext c);
#endif

#if defined(WIN32)
void SDLController::MakeTransparent_Windows (const SDL_SysWMinfo &sys_wm_info) {
  HWND hWnd = sys_wm_info.info.win.window;
  if (hWnd) {
    LONG flags = ::GetWindowLongA(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    if (m_Params.alwaysOnTop) {
      flags |= WS_EX_TOPMOST;
    }
    ::SetWindowLongA(hWnd, GWL_EXSTYLE, flags);
    ::SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
  } else {
    throw std::runtime_error("Error retrieiving native window");
  }
  if (m_Params.alwaysOnTop) {
    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
  DWM_BLURBEHIND bb = { 0 };
  bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
  bb.fEnable = true;
  bb.hRgnBlur = CreateRectRgn(0, 0, 1, 1);
  ::DwmEnableBlurBehindWindow(hWnd, &bb);
}
#endif

void SDLController::InitSDL() {
  // Initialize SDL.  TODO: initialize audio
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLController::ConfigureFrameBuffer() {
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
}

void SDLController::ConfigureAntialiasing() {
  if (m_Params.antialias) {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
  }
}

void SDLController::InitWindow() {
  if (m_Params.fullscreen) {
    SDL_Rect rect;
    if (SDL_GetDisplayBounds(0, &rect) == 0) {
      m_Params.windowPosX = 0;
      m_Params.windowPosY = 0;
      m_Params.windowWidth = rect.w;
      m_Params.windowHeight = rect.h-1;
    }
  }

  if (m_Params.transparentWindow) {
    // Transparent window only works properly when the window is borderless
    m_Params.windowFlags |= SDL_WINDOW_BORDERLESS;
  }

  // Create a window.
  m_SDL_Window = SDL_CreateWindow(m_Params.windowTitle.c_str(), m_Params.windowPosX, m_Params.windowPosY, m_Params.windowWidth, m_Params.windowHeight, m_Params.windowFlags);

  if (m_SDL_Window == nullptr) {
    CleanUpInitializedResources();
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLController::ConfigureRenderer() {
  // Create a renderer.  NOTE: it may be platform-specific as to if we want V-sync.
  Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
  if (m_Params.vsync) {
    rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
  }

  m_SDL_Renderer = SDL_CreateRenderer(m_SDL_Window, -1, rendererFlags);
  if (m_SDL_Renderer == nullptr) {
    CleanUpInitializedResources();
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLController::InitGLContext() {
  // Create a GL context.
  m_SDL_GLContext = SDL_GL_CreateContext(m_SDL_Window);
  if (m_SDL_GLContext == nullptr) {
    CleanUpInitializedResources();
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLController::InitGlew() {
  if (glewInit() != GLEW_OK) {
    CleanUpInitializedResources();
    throw std::runtime_error("Glew initialization failed");
  }
}

void SDLController::ConfigureTransparentWindow() {
  struct SDL_SysWMinfo sys_wm_info;
  SDL_VERSION(&sys_wm_info.version);

  // Retrieve the window info.
  if (!SDL_GetWindowWMInfo(m_SDL_Window, &sys_wm_info)) {
    CleanUpInitializedResources();
    throw std::runtime_error("Error retrieving window WM info");
  }

  // Make the OS-specific call to make the window transparent.
  switch (sys_wm_info.subsystem) {
#ifdef WIN32 
  case SDL_SYSWM_WINDOWS:
    m_HWND = sys_wm_info.info.win.window;
    if (m_Params.transparentWindow) {
      MakeTransparent_Windows(sys_wm_info);
    }
    break;
#elif __APPLE__ 
  case SDL_SYSWM_COCOA:
    if (m_Params.transparentWindow) {
      MakeTransparent_Apple(sys_wm_info, m_SDL_GLContext);
    }
    break;
#else 
  case SDL_SYSWM_X11:
    // TODO
    break;
#endif 
  default:
    CleanUpInitializedResources();
    throw std::runtime_error("Error identifying WM subsystem");
    break;
  }
}

void SDLController::CleanUpInitializedResources() {
  // TODO: shutdown anything else necessary, video, and SDL (in reverse order as init)
  if (m_SDL_GLContext != nullptr) {
    SDL_GL_DeleteContext(m_SDL_GLContext);
    m_SDL_GLContext = nullptr;
  }
  if (m_SDL_Renderer != nullptr) {
    SDL_DestroyRenderer(m_SDL_Renderer);
    m_SDL_Renderer = nullptr;
  }
  if (m_SDL_Window != nullptr) {
    SDL_DestroyWindow(m_SDL_Window);
    m_SDL_Window = nullptr;
  }
  SDL_Quit();
}

void SDLController::ResizeWindow(int width, int height){
  if (m_SDL_Window == nullptr) {
    throw std::runtime_error("Error resizing SDL window");
  }
  SDL_SetWindowSize(m_SDL_Window, width, height);
}

Uint32 SDLController::GetWindowID(){
  if (m_SDL_Window == nullptr) {
    throw std::runtime_error("Error getting SDL window ID");
  }
  return SDL_GetWindowID(m_SDL_Window);
}
