#pragma once

#include "Leap/Hmd/DeviceConfiguration.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Leap {
namespace OculusRift {

class DeviceConfiguration : public Hmd::DeviceConfiguration {
public:

  virtual ~DeviceConfiguration () { }

  virtual std::string Identifier () const override { return "OculusRift"; } // TODO
  virtual float InterPupillaryDistance () const override { return 56.0f; } // TODO
  virtual uint32_t EyeCount () const override { return 2; } // TODO
  virtual std::vector<uint32_t> EyeRenderOrder () const override { return std::vector<uint32_t>{0,1}; } // TODO
  // virtual std::shared_ptr<Leap::Hmd::EyeConfiguration> EyeConfiguration (uint32_t eye_index) const = 0;
};

} // end of namespace OculusRift
} // end of namespace Leap
