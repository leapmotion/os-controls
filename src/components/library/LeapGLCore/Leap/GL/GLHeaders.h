#pragma once

// this header is only intended to take care of the annoying OpenGL header includes in a cross-platform way.

#if __APPLE__
  #include <GL/glew.h>
#elif _WIN32
  #define GLEW_STATIC
  #include <GL/glew.h>

  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif

  #ifndef NOMINMAX
    #define NOMINMAX
  #endif

  #include "windows.h"
#else
  #include <GL/glew.h>
#endif

#include <iostream>
#include "Leap/GL/Exception.h"

namespace Leap {
namespace GL {

inline void InitializeGlew () {
  if (glewInit() != GLEW_OK) {
    throw Leap::GL::Exception("Glew initialization failed");
  }
  std::cerr << "GL_VERSION = \"" << glGetString(GL_VERSION) << "\"\n";       // TEMP
  std::cerr << "GL_RENDERER = \"" << glGetString(GL_RENDERER) << "\"\n";     // TEMP
  std::cerr << "GL_VENDOR = \"" << glGetString(GL_VENDOR) << "\"\n";         // TEMP
  // std::cerr << "GL_EXTENSIONS = \"" << glGetString(GL_EXTENSIONS) << "\"\n"; // TEMP
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
