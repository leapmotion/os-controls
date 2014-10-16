#pragma once

#include <stdexcept>
#include <string>

namespace Leap {
namespace Hmd {

class Context;
class Device;

class Exception : std::exception {
public:

  virtual ~Exception () { }

  // Returns the Hmd::Context to which this Exception applies, or nullptr if not specified/defined.
  virtual const Hmd::Context *Context () const = 0;
  // Returns the Hmd::Device to which this Exception applies, or nullptr if not specified/defined.
  virtual const Hmd::Device *Device () const = 0;
};

} // end of namespace Hmd
} // end of namespace Leap
