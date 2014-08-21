#pragma once

// this header is only intended to take care of the annoying OpenGL header includes in a cross-platform way.

#if __APPLE__
  #include <GL/glew.h>
#elif _WIN32
  #define GLEW_STATIC
  #include <GL/glew.h>
  #include "windows.h"
#else
  #include <GL/glew.h>
  // TODO: get rid of these includes below, because glew should moot them.
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
