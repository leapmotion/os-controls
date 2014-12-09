#include "Leap/GL/GLHeaders.h"
#include "GLTestFramework.h"

using namespace Leap::GL;

// This class will initialize and shutdown a GL context (SDL based) respectively before and after running each test.
GLTestFramework::GLTestFramework (Visibility visibility) {
  if (visibility == Visibility::HEADLESS) {
    // Cause the window to not be shown.
    m_SDLControllerParams.windowFlags &= ~SDL_WINDOW_SHOWN;  // Disable the "shown" flag.
    m_SDLControllerParams.windowFlags |=  SDL_WINDOW_HIDDEN; // Enable the "hidden" flag.
  }
}

GLTestFramework::~GLTestFramework () { }
  
void GLTestFramework::SetUp () {
  m_SDLController.Initialize(m_SDLControllerParams);
  InitializeGlew();

  BeginFrame();
}

void GLTestFramework::BeginFrame () {
  m_SDLController.BeginRender();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // if using transparent window, clear alpha value must be 0
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLTestFramework::EndFrame () {
  glFlush();
  m_SDLController.EndRender();  
}

void GLTestFramework::TearDown () {
  EndFrame();

  // Shut down the created things in reverse order.
  // Nothing to do in shutting down Glew.
  m_SDLController.Shutdown();
}
