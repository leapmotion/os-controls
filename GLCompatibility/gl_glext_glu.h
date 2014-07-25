#pragma once

// this header is only intended to take care of the annoying OpenGL header includes in a cross-platform way.

#if __APPLE__
  #include <GL/glew.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#elif _WIN32
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include "windows.h"
#else
  #include <GL/glew.h>
  #include <gl/gl.h>
  #include <gl/glu.h>
#endif

// TEMPORARY shim until std::is_same from <type_traits> is available
template <typename T0_, typename T1_> struct is_same { static const bool value = false; };
template <typename T_> struct is_same<T_,T_> { static const bool value = true; };

