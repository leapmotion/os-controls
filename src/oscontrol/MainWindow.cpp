#include "MainWindow.h"

#include <cassert>

#include "ExampleLayer.h"
#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "gl_glext_glu.h"
#include <autowiring/autowiring.h>

MainWindow::MainWindow () {
  // Most everything that would normally go in a constructor should really go in Initialize().
}

MainWindow::~MainWindow () {
  // Most everything that would normally go in a destructor should really go in Shutdown().
}

void MainWindow::Initialize() {
  m_applicationTime = TimePoint(0.0);         // Start the application time at zero.
  m_SDLController.Initialize(true);           // This initializes everything SDL-related.
  m_GLController.Initialize();                // This initializes the general GL state.
  FreeImage_Initialise();                     // Initialize FreeImage.
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Glew initialization failed");
  }
  InitializeApplicationLayers();              // Initialize the application layers (contents of the app).

  // TODO: initialize other threads here?
}

void MainWindow::Shutdown() {
  ShutdownApplicationLayers();                // Destroy the application layers, from top (last) to bottom (first).
  FreeImage_DeInitialise();                   // Shut down FreeImage.
  m_GLController.Shutdown();                  // This shuts down the general GL state.
  m_SDLController.Shutdown();                 // This shuts down everything SDL-related.
}

void MainWindow::Update(TimeDelta real_time_delta) {
  assert(real_time_delta >= 0.0);
  m_applicationTime += real_time_delta;         // Increment the application time by the delta.

  for (auto it = m_applicationLayers.begin(); it != m_applicationLayers.end(); ++it) {
    RenderableEventHandler &layer = **it;
    layer.Update(real_time_delta);              // Update each application layer, from back to front.
  }
}

void MainWindow::Render(TimeDelta real_time_delta) const {
  assert(real_time_delta >= 0.0);

  m_SDLController.BeginRender();
  m_GLController.BeginRender();               // NOTE: ALL RENDERING should go between here and EndRender().

  for (auto it = m_applicationLayers.begin(); it != m_applicationLayers.end(); ++it) {
    RenderableEventHandler &layer = **it;
    layer.Render(real_time_delta);              // Render each application layer, from back to front.
  }

  m_GLController.EndRender();                 // NOTE: ALL RENDERING should go between here and BeginRender().
  m_SDLController.EndRender();
}

EventHandlerAction MainWindow::HandleWindowEvent(const SDL_WindowEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_WindowEvent>(ev, &EventHandler::HandleWindowEvent);
}

EventHandlerAction MainWindow::HandleKeyboardEvent(const SDL_KeyboardEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_KeyboardEvent>(ev, &EventHandler::HandleKeyboardEvent);
}

EventHandlerAction MainWindow::HandleMouseMotionEvent(const SDL_MouseMotionEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseMotionEvent>(ev, &EventHandler::HandleMouseMotionEvent);
}

EventHandlerAction MainWindow::HandleMouseButtonEvent(const SDL_MouseButtonEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseButtonEvent>(ev, &EventHandler::HandleMouseButtonEvent);
}

EventHandlerAction MainWindow::HandleMouseWheelEvent(const SDL_MouseWheelEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_MouseWheelEvent>(ev, &EventHandler::HandleMouseWheelEvent);
}

EventHandlerAction MainWindow::HandleQuitEvent(const SDL_QuitEvent &ev) {
  return DispatchEventToApplicationLayers<SDL_QuitEvent>(ev, &EventHandler::HandleQuitEvent);
}

EventHandlerAction MainWindow::HandleGenericSDLEvent(const SDL_Event &ev) {
  return DispatchEventToApplicationLayers<SDL_Event>(ev, &EventHandler::HandleGenericSDLEvent);
}

TimePoint MainWindow::Time () const {
  return m_applicationTime;
}

void MainWindow::InitializeApplicationLayers () {
  m_applicationLayers.push_back(std::make_shared<ExampleLayer>());
  // TODO: create GUI layer, title screen
}

void MainWindow::ShutdownApplicationLayers () {
  // Destroy the application-specific layers, in reverse order.
  for (auto it = m_applicationLayers.rbegin(); it != m_applicationLayers.rend(); ++it) {
    it->reset();  // destroy the layer by resetting its shared_ptr.
  }
  m_applicationLayers.clear();
}
