#include "GLTestFramework.h"

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
  m_GLController.Initialize();
  
  m_SDLController.BeginRender();
  m_GLController.BeginRender();
}

void GLTestFramework::TearDown () {
  m_GLController.EndRender();
  m_SDLController.EndRender();
  
  // Shut down the created things in reverse order
  m_GLController.Shutdown();
  m_SDLController.Shutdown();
}
