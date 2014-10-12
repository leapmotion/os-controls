#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Leap {
namespace Hmd {

class DeviceConfiguration {
public:

  virtual ~DeviceConfiguration () { }

  virtual std::string Identifier () const = 0;
  virtual float InterPupillaryDistance () const = 0;
  virtual uint32_t EyeCount () const = 0;
  virtual std::vector<uint32_t> EyeRenderOrder () const = 0;
  virtual std::shared_ptr<Leap::Hmd::EyeConfiguration> EyeConfiguration (uint32_t eye_index) const = 0;
};

} // end of namespace Hmd
} // end of namespace Leap
