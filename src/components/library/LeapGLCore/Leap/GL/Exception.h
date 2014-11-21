#pragma once

#include <stdexcept>
#include <string>

namespace Leap {
namespace GL {

// Base class for all Leap::GL exceptions.  TODO: Rename to Exception.
class GLException : public std::runtime_error {
public:
  GLException (const std::string &message) : std::runtime_error(message) { }
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
