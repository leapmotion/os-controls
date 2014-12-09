#pragma once

#include "Leap/GL/Exception.h"

#ifndef LEAP_NOEXCEPT
  #if _WIN32
    #define LEAP_NOEXCEPT
  #else
    #define LEAP_NOEXCEPT noexcept
  #endif
#endif

namespace Leap {
namespace GL {

// Base class for all Leap::GL::Texture2 exceptions.
class Texture2Exception : public Exception {
public:

  Texture2Exception (const std::string &message) : Exception(message) { }
};

} // end of namespace GL
} // end of namespace Leap
