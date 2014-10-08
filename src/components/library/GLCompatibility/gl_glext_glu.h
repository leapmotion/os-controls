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
