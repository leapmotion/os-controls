#pragma once

namespace Leap {
namespace Hmd {

#include <stdexcept>
#include <string>

class Exception : std::exception {
public:

  Exception (const std::string &message) : std::exception(message) { }
  virtual ~Exception () { }

  // TODO: add data about which Context, Device, etc this came from?
};

} // end of namespace Hmd
} // end of namespace Leap
