#include "ShapesApplication.h"

#include <cassert>

#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "Leap/GL/GLHeaders.h"
// There is a really dumb compile error on Linux: "Eigen/src/Core/util/Constants.h:369:2:
// error: #error The preprocessor symbol 'Success' is defined, possibly by the X11 header file X.h",
// so this undef is necessary until we can figure out a better solution.
#if defined(Success)
#undef Success
#endif
#include "MediaControlLayer.h"
#include "ShapesLayer.h"
#include "AnimationLayer.h"

using namespace Leap::GL;

ShapesApplication::ShapesApplication () {
  // Most everything that would normally go in a constructor should really go in Initialize().
}

ShapesApplication::~ShapesApplication () {
  // Most everything that would normally go in a destructor should really go in Shutdown().
}

void ShapesApplication::Initialize() {
  SFMLControllerParams params;
#if !__linux__
  params.transparentWindow = true;
  params.fullscreen = true;
#endif
  params.alwaysOnTop = true;
  params.windowTitle = "Primitives Test";
  params.antialias = true;
  params.vsync = true;

  m_applicationTime = TimePoint(0.0);         // Start the application time at zero.
  m_SFMLController.Initialize(params);        // This initializes everything SDL-related.
  InitializeGlew(&std::cerr);                 // This initializes the general GL state. -- NOTE: this might already be done by SFML
  FreeImage_Initialise();                     // Initialize FreeImage.
  InitializeApplicationLayers();              // Initialize the application layers (contents of the app).

  // TODO: initialize other threads here?
}

void ShapesApplication::Shutdown() {
  ShutdownApplicationLayers();                // Destroy the application layers, from top (last) to bottom (first).
  FreeImage_DeInitialise();                   // Shut down FreeImage.
  m_SFMLController.Shutdown();                // This shuts down everything SDL-related.
}

void ShapesApplication::Update(TimeDelta real_time_delta) {
  assert(real_time_delta >= 0.0);
  m_applicationTime += real_time_delta;         // Increment the application time by the delta.

  for (auto it = m_applicationLayers.begin(); it != m_applicationLayers.end(); ++it) {
    RenderableEventHandler &layer = **it;
    layer.Update(real_time_delta);              // Update each application layer, from back to front.
  }
}

void ShapesApplication::Render(TimeDelta real_time_delta) const {
  assert(real_time_delta >= 0.0);

  m_SFMLController.BeginRender();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // if using transparent window, clear alpha value must be 0
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // NOTE: ALL RENDERING should go between here and the ending marker

  for (auto it = m_applicationLayers.begin(); it != m_applicationLayers.end(); ++it) {
    RenderableEventHandler &layer = **it;
    layer.Render(real_time_delta);              // Render each application layer, from back to front.
  }

  // NOTE: ALL RENDERING should end by this ending marker.
  glFlush();
  m_SFMLController.EndRender();
}

EventHandlerAction ShapesApplication::HandleWindowEvent(const SDL_WindowEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_WindowEvent>(ev, &EventHandler::HandleWindowEvent);
}

EventHandlerAction ShapesApplication::HandleKeyboardEvent(const SDL_KeyboardEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_KeyboardEvent>(ev, &EventHandler::HandleKeyboardEvent);
}

EventHandlerAction ShapesApplication::HandleMouseMotionEvent(const SDL_MouseMotionEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseMotionEvent>(ev, &EventHandler::HandleMouseMotionEvent);
}

EventHandlerAction ShapesApplication::HandleMouseButtonEvent(const SDL_MouseButtonEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseButtonEvent>(ev, &EventHandler::HandleMouseButtonEvent);
}

EventHandlerAction ShapesApplication::HandleMouseWheelEvent(const SDL_MouseWheelEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseWheelEvent>(ev, &EventHandler::HandleMouseWheelEvent);
}

EventHandlerAction ShapesApplication::HandleQuitEvent(const SDL_QuitEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_QuitEvent>(ev, &EventHandler::HandleQuitEvent);
}

EventHandlerAction ShapesApplication::HandleGenericSDLEvent(const SDL_Event &ev) {
  return DispatchEventToApplicationLayers<SDL_Event>(ev, &EventHandler::HandleGenericSDLEvent);
}

TimePoint ShapesApplication::Time () const {
  return m_applicationTime;
}

void ShapesApplication::InitializeApplicationLayers () {
  std::shared_ptr<ShapesLayer> layer(new ShapesLayer());
  //std::shared_ptr<MediaControlLayer> layer(new MediaControlLayer());
  //std::shared_ptr<AnimationLayer> layer(new AnimationLayer());
  layer->SetDimensions(m_SFMLController.GetParams().windowWidth, m_SFMLController.GetParams().windowHeight);
  m_applicationLayers.push_back(layer);
  // TODO: create GUI layer, title screen
}

void ShapesApplication::ShutdownApplicationLayers () {
  // Destroy the application-specific layers, in reverse order.
  for (auto it = m_applicationLayers.rbegin(); it != m_applicationLayers.rend(); ++it) {
    it->reset();  // destroy the layer by resetting its shared_ptr.
  }
  m_applicationLayers.clear();
}
