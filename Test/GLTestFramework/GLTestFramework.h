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

// TODO: put this in a better place -- it doesn't depend on any GL stuff, only GTest.

// Because GTest's ASSERT_NO_THROW doesn't print a message if available.
#define EXPECT_NO_THROW_(x) \
  try { \
    x; \
  } catch (const std::exception &e) { \
    std::cerr << "exception caught: " << e.what() << '\n'; \
    EXPECT_TRUE(false) << e.what(); \
  } catch (...) { \
    std::cerr << "exception caught (no message)\n"; \
    EXPECT_TRUE(false); \
  }

// Because GTest's ASSERT_NO_THROW doesn't print a message if available.
#define ASSERT_NO_THROW_(x) \
  try { \
    x; \
  } catch (const std::exception &e) { \
    std::cerr << "exception caught: " << e.what() << '\n'; \
    ASSERT_TRUE(false) << e.what(); \
  } catch (...) { \
    std::cerr << "exception caught (no message)\n"; \
    ASSERT_TRUE(false); \
  }

