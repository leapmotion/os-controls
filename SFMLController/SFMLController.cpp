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
}

void SFMLController::Initialize(const SFMLControllerParams& params) {
  m_Params = params;

  InitWindow();
}

void SFMLController::Shutdown () {
}

void SFMLController::BeginRender () const {
  // Do whatever needs to be done before rendering a frame.
}

void SFMLController::EndRender () const {
  m_Window.display();
}

std::string SFMLController::BasePath () {
  return "";
}

// It's necessary to put the Apple-specific code in a separate file because
// it is Objective-C++ and needs to be compiled as such (the source file name
// is MakeTransparent_Apple.mm).
#if __APPLE__
extern void MakeTransparent_Apple (const SDL_SysWMinfo &sys_wm_info, SDL_GLContext c);
#endif

#if defined(WIN32)
void SFMLController::MakeTransparent_Windows () {
  sf::WindowHandle handle = m_Window.getSystemHandle();
  HWND hWnd = static_cast<HWND>(handle);
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

void SFMLController::InitWindow() {
  sf::Uint32 windowStyle = sf::Style::Default;
  if (m_Params.transparentWindow) {
    windowStyle = sf::Style::None;
  }

  m_Settings.antialiasingLevel = 0;
  if (m_Params.antialias) {
    m_Settings.antialiasingLevel = 16;
  }

  if (m_Params.fullscreen) {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    m_Params.windowPosX = 0;
    m_Params.windowPosY = 0;
    m_Params.windowWidth = desktopMode.width;
    m_Params.windowHeight = desktopMode.height-1;
  }

  m_Settings.depthBits = 24;

  m_Window.create(sf::VideoMode(m_Params.windowWidth, m_Params.windowHeight, 32U), m_Params.windowTitle, windowStyle, m_Settings);

  m_Window.setVerticalSyncEnabled(m_Params.vsync);

  if (m_Params.transparentWindow) {
    // Make the OS-specific call to make the window transparent.
#ifdef WIN32 
    MakeTransparent_Windows();
#elif __APPLE__ 
    MakeTransparent_Apple(sys_wm_info, m_SDL_GLContext);
#else 
    //TODO
#endif
  }
}
