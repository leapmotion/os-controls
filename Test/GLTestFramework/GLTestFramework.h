#pragma once

#include "GLController.h"
#include <gtest/gtest.h>
#include "SDLController.h"

// This class will initialize and shutdown a GL context (SDL based) respectively before and after running each test.
class GLTestFramework : public testing::Test {
protected:

  enum class Visibility { VISIBLE, HEADLESS };
  
  GLTestFramework (Visibility visibility) {
    if (visibility == Visibility::HEADLESS) {
      // Cause the window to not be shown.
      m_SDLControllerParams.windowFlags &= ~SDL_WINDOW_SHOWN;  // Disable the "shown" flag.
      m_SDLControllerParams.windowFlags |=  SDL_WINDOW_HIDDEN; // Enable the "hidden" flag.
    }
  }
  virtual ~GLTestFramework () { }
  
  virtual void SetUp () override;
  virtual void TearDown () override;

protected:

  SDLControllerParams m_SDLControllerParams;
  SDLController m_SDLController;
  GLController m_GLController;
};

void GLTestFramework::SetUp () {
  m_SDLController.Initialize(m_SDLControllerParams);
  m_GLController.Initialize();
}

void GLTestFramework::TearDown () {
  // Shut down the created things in reverse order
  m_GLController.Shutdown();
  m_SDLController.Shutdown();
}

// For creating a GLTestFramework that renders to a visible window
class GLTestFramework_Visible : public GLTestFramework {
public:
  
  GLTestFramework_Visible () : GLTestFramework(Visibility::VISIBLE) { }
  virtual ~GLTestFramework_Visible () { }
};

// For creating a GLTestFramework 
class GLTestFramework_Headless : public GLTestFramework {
public:
  
  GLTestFramework_Headless () : GLTestFramework(Visibility::HEADLESS) { }
  virtual ~GLTestFramework_Headless () { }
};
