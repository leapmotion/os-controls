#pragma once

#include <stdexcept>
#include <string>

#ifndef LEAP_NOEXCEPT
  #if _WIN32
    #define LEAP_NOEXCEPT
  #else
    #define LEAP_NOEXCEPT noexcept
  #endif
#endif

namespace Leap {
namespace GL {

// Base class for all Leap::GL exceptions.  TODO: Rename to Exception.
class GLException : public std::exception {
public:

  GLException (const std::string &message) : m_message(message) { }

  virtual const char *what () const LEAP_NOEXCEPT override { return m_message.c_str(); }

private:

  std::string m_message;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
