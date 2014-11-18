#include "SFMLController.h"
#include <SFML/Window/Event.hpp>

#if _WIN32
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#elif __APPLE__
#include <AppKit/NSColor.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSWindow.h>
#include <AppKit/NSView.h>
#include <objc/runtime.h>
#include <mutex>
#endif

#include <cassert>
#include <stdexcept>

SFMLController::SFMLController() {
#if __APPLE__
  static std::once_flag s_flag;
  std::call_once(s_flag, [] {
    //
    // The isOpaque method in the SFOpenGLView class of SFML always returns YES
    // (as it just uses the default implementation of NSOpenGLView). This
    // causes us to always get an opaque view. We workaround this problem by
    // replacing that method with our own implementation that returns the
    // opaqueness based on the enclosing window, all thanks to the power of
    // Objective-C.
    //
    method_setImplementation(class_getInstanceMethod(NSClassFromString(@"SFOpenGLView"), @selector(isOpaque)),
                            imp_implementationWithBlock(^BOOL(id self, id arg) { return [[self window] isOpaque]; }));
  });
#endif
}

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
  sf::Event event;
  while (m_Window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      m_Window.close();
    }
  }
}

void SFMLController::EndRender () const {
  m_Window.display();
}

std::string SFMLController::BasePath () {
  return "";
}

void SFMLController::MakeTransparent () {
#if _WIN32
  if (m_HWND) {
    LONG flags = ::GetWindowLongA(m_HWND, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    if (m_Params.alwaysOnTop) {
      flags |= WS_EX_TOPMOST;
    }
    ::SetWindowLongA(m_HWND, GWL_EXSTYLE, flags);
    ::SetLayeredWindowAttributes(m_HWND, RGB(0, 0, 0), 255, LWA_ALPHA);
  } else {
    throw std::runtime_error("Error retrieving native window");
  }
  if (m_Params.alwaysOnTop) {
    ::SetWindowPos(m_HWND, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
  DWM_BLURBEHIND bb = { 0 };
  bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
  bb.fEnable = true;
  bb.hRgnBlur = CreateRectRgn(0, 0, 1, 1);
  ::DwmEnableBlurBehindWindow(m_HWND, &bb);
#elif __APPLE__
  NSWindow* window = static_cast<NSWindow*>(m_WindowHandle);
  NSOpenGLView* view = [window contentView];

  if (!window || !view) {
    throw std::runtime_error("Error retrieving native window");
  }

  // Set the GL context opacity.
  NSOpenGLContext* context = [view openGLContext];
  // The opacity var should technically be a GLint, but from
  // http://www.opengl.org/wiki/OpenGL_Type -- GLint is necessarily 32 bit,
  // so we can use a fixed int type without including any GL headers here.
  int32_t opacity = 0;
  [context setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];

  // Set window properties.
  [window setOpaque:NO];
  [window setHasShadow:NO];
  [window setHidesOnDeactivate:NO];
  [window setBackgroundColor:[NSColor clearColor]];
  [window setBackingType:NSBackingStoreBuffered];
  [window setSharingType:NSWindowSharingNone];
  [window setLevel:CGShieldingWindowLevel()];
  [window setCollectionBehavior:(NSWindowCollectionBehaviorCanJoinAllSpaces |
                                 NSWindowCollectionBehaviorStationary |
                                 NSWindowCollectionBehaviorFullScreenAuxiliary |
                                 NSWindowCollectionBehaviorIgnoresCycle)];
  [window display];
#else
  throw std::runtime_error("Missing implementation of MakeTransparent");
#endif
}

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
  m_Window.setVisible(false);
  m_Window.setVerticalSyncEnabled(m_Params.vsync);
  m_Window.setPosition(sf::Vector2i(m_Params.windowPosX, m_Params.windowPosY));

  m_WindowHandle = m_Window.getSystemHandle();
#if _WIN32
  m_HWND = static_cast<HWND>(m_WindowHandle);
#endif
  if (m_Params.transparentWindow) {
    // Make the OS-specific call to make the window transparent.
    MakeTransparent();
  }
  m_Window.setVisible(true);
}
