#pragma once

#include "GLController.h"
#include <gtest/gtest.h>
#include "SDLController.h"

// This class will initialize and shutdown a GL context (SDL based) respectively before and after running each test.
class GLTestFramework : public testing::Test {
protected:

  enum class Visibility { VISIBLE, HEADLESS };
  
  GLTestFramework (Visibility visibility);
  virtual ~GLTestFramework ();
  
  virtual void SetUp () override;
  virtual void TearDown () override;

protected:

  SDLControllerParams m_SDLControllerParams;
  SDLController m_SDLController;
  GLController m_GLController;
};

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
