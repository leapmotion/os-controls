#include "OculusApplication.h"

#include <cassert>
#include <iostream>

#include "Leap/GL/GLHeaders.h"

#if _WIN32
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif

OculusApplication::OculusApplication () {
  // Most everything that would normally go in a constructor should really go in Initialize().
}

OculusApplication::~OculusApplication () {
  // Most everything that would normally go in a destructor should really go in Shutdown().
}

void OculusApplication::Initialize() {
  SFMLControllerParams params;
  params.transparentWindow = false;
  params.alwaysOnTop = false;
  params.fullscreen = false;
  params.windowTitle = "Oculus Sample";
  params.antialias = true;
  params.vsync = true;

  m_applicationTime = TimePoint(0.0);         // Start the application time at zero.
  m_SFMLController.Initialize(params);        // This initializes everything SFML-related.

#if _WIN32
  m_Oculus.SetWindow(m_SFMLController.GetHWND());
#endif

  if (!m_Oculus.Init()) {
    throw std::runtime_error("Oculus initialization failed");
  }
}

void OculusApplication::Shutdown() {
  m_SFMLController.Shutdown();                // This shuts down everything SFML-related.
}

void OculusApplication::Update(TimeDelta real_time_delta) {
  assert(real_time_delta >= 0.0);
  m_applicationTime += real_time_delta;       // Increment the application time by the delta.
}

void OculusApplication::Render(TimeDelta real_time_delta) const {
  assert(real_time_delta >= 0.0);

  m_SFMLController.BeginRender();
  m_Oculus.BeginFrame();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // NOTE: ALL RENDERING should go between here and the ending marker

  for (int i=0; i<2; i++) {
    const ovrRecti& rect = m_Oculus.EyeViewport(i);
    const EigenTypes::Matrix4x4f proj = m_Oculus.EyeProjection(i);
    const EigenTypes::Matrix4x4f view = m_Oculus.EyeView(i);

    glViewport(rect.Pos.x, rect.Pos.y, rect.Size.w, rect.Size.h);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj.data());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(view.data());

    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    for (double x = -100; x <= 100; x+=5) {
      for (double y = -100; y <= 100; y+=5) {
        glVertex3d(x, 0, -100);
        glVertex3d(x, 0, 100);
        glVertex3d(-100, 0, y);
        glVertex3d(100, 0, y);
      }
    }
    glEnd();
  }

  m_Oculus.EndFrame();

#if 0 // for SDK rendering, disable our buffer swap
  // NOTE: ALL RENDERING should end by this ending marker.
  glFlush();
  m_SFMLController.EndRender();
#endif
}

EventHandlerAction OculusApplication::HandleWindowEvent(const SDL_WindowEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_WindowEvent>(ev, &EventHandler::HandleWindowEvent);
}

EventHandlerAction OculusApplication::HandleKeyboardEvent(const SDL_KeyboardEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_KeyboardEvent>(ev, &EventHandler::HandleKeyboardEvent);
}

EventHandlerAction OculusApplication::HandleMouseMotionEvent(const SDL_MouseMotionEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseMotionEvent>(ev, &EventHandler::HandleMouseMotionEvent);
}

EventHandlerAction OculusApplication::HandleMouseButtonEvent(const SDL_MouseButtonEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseButtonEvent>(ev, &EventHandler::HandleMouseButtonEvent);
}

EventHandlerAction OculusApplication::HandleMouseWheelEvent(const SDL_MouseWheelEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseWheelEvent>(ev, &EventHandler::HandleMouseWheelEvent);
}

EventHandlerAction OculusApplication::HandleQuitEvent(const SDL_QuitEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_QuitEvent>(ev, &EventHandler::HandleQuitEvent);
}

EventHandlerAction OculusApplication::HandleGenericSDLEvent(const SDL_Event &ev) {
  return DispatchEventToApplicationLayers<SDL_Event>(ev, &EventHandler::HandleGenericSDLEvent);
}

TimePoint OculusApplication::Time () const {
  return m_applicationTime;
}
