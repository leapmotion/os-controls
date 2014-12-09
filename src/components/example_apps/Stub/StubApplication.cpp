#include "StubApplication.h"

#include <cassert>

#include "ExampleLayer.h"
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "Leap/GL/GLHeaders.h"
#include "ResourceManager.h"
#include "Singleton.h"

namespace Leap {
namespace GL {

class Texture2;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL;

StubApplication::StubApplication () {
  // Most everything that would normally go in a constructor should really go in Initialize().
}

StubApplication::~StubApplication () {
  // Most everything that would normally go in a destructor should really go in Shutdown().
}

void StubApplication::Initialize() {
  SDLControllerParams params;
#if !__linux__
  params.transparentWindow = true;
#endif
  params.alwaysOnTop = false;
  params.fullscreen = false;
  params.windowTitle = "Stub App";
  // params.antialias = true; // antialiasing was not working on my machine, so leaving it at its default value of false.
  params.vsync = true;

  m_applicationTime = TimePoint(0.0);         // Start the application time at zero.
  m_SDLController.Initialize(params);         // This initializes everything SDL-related.

  // Create the ResourceManager<Texture2> with the SDL-supplied basepath.  This
  // must be done before the ResourceManager<Texture2> is referred to by anything
  // else (e.g. Resource<Texture2>).
  Singleton<ResourceManager<Texture2>>::CreateInstance(SDLController::BasePath());

  InitializeGlew();                           // This initializes the general GL state.
  FreeImage_Initialise();                     // Initialize FreeImage.
  InitializeApplicationLayers();              // Initialize the application layers (contents of the app).

  // TODO: initialize other threads here?
}

void StubApplication::Shutdown() {
  ShutdownApplicationLayers();                // Destroy the application layers, from top (last) to bottom (first).
  FreeImage_DeInitialise();                   // Shut down FreeImage.
  m_SDLController.Shutdown();                 // This shuts down everything SDL-related.
}

void StubApplication::Update(TimeDelta real_time_delta) {
  assert(real_time_delta >= 0.0);
  m_applicationTime += real_time_delta;       // Increment the application time by the delta.

  for (auto it = m_applicationLayers.begin(); it != m_applicationLayers.end(); ++it) {
    RenderableEventHandler &layer = **it;
    layer.Update(real_time_delta);            // Update each application layer, from back to front.
  }
}

void StubApplication::Render(TimeDelta real_time_delta) const {
  assert(real_time_delta >= 0.0);

  m_SDLController.BeginRender();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // if using transparent window, clear alpha value must be 0
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // NOTE: ALL RENDERING should go between here and the ending marker

  for (auto it = m_applicationLayers.begin(); it != m_applicationLayers.end(); ++it) {
    RenderableEventHandler &layer = **it;
    layer.Render(real_time_delta);            // Render each application layer, from back to front.
  }

  // NOTE: ALL RENDERING should end by this ending marker.
  
  glFlush();
  m_SDLController.EndRender();
}

EventHandlerAction StubApplication::HandleWindowEvent(const SDL_WindowEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_WindowEvent>(ev, &EventHandler::HandleWindowEvent);
}

EventHandlerAction StubApplication::HandleKeyboardEvent(const SDL_KeyboardEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_KeyboardEvent>(ev, &EventHandler::HandleKeyboardEvent);
}

EventHandlerAction StubApplication::HandleMouseMotionEvent(const SDL_MouseMotionEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseMotionEvent>(ev, &EventHandler::HandleMouseMotionEvent);
}

EventHandlerAction StubApplication::HandleMouseButtonEvent(const SDL_MouseButtonEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseButtonEvent>(ev, &EventHandler::HandleMouseButtonEvent);
}

EventHandlerAction StubApplication::HandleMouseWheelEvent(const SDL_MouseWheelEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseWheelEvent>(ev, &EventHandler::HandleMouseWheelEvent);
}

EventHandlerAction StubApplication::HandleQuitEvent(const SDL_QuitEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_QuitEvent>(ev, &EventHandler::HandleQuitEvent);
}

EventHandlerAction StubApplication::HandleGenericSDLEvent(const SDL_Event &ev) {
  return DispatchEventToApplicationLayers<SDL_Event>(ev, &EventHandler::HandleGenericSDLEvent);
}

TimePoint StubApplication::Time () const {
  return m_applicationTime;
}

void StubApplication::InitializeApplicationLayers () {
  m_applicationLayers.push_back(std::make_shared<ExampleLayer>());
  // TODO: create GUI layer, title screen
}

void StubApplication::ShutdownApplicationLayers () {
  // Destroy the application-specific layers, in reverse order.
  for (auto it = m_applicationLayers.rbegin(); it != m_applicationLayers.rend(); ++it) {
    it->reset();  // destroy the layer by resetting its shared_ptr.
  }
  m_applicationLayers.clear();
}
