#pragma once

#include "Leap/Hmd/DeviceConfiguration.h"
#include "Leap/OculusRift/EyeConfiguration.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Leap {
namespace OculusRift {

class Context;
class Device;

class DeviceConfiguration : public Hmd::IDeviceConfiguration {
public:

  DeviceConfiguration (
    const OculusRift::Context &context,
    const OculusRift::Device &device,
    const std::string &identifier,
    float inter_pupillary_distance,
    const std::vector<uint32_t> &eye_render_order,
    const std::vector<OculusRift::EyeConfiguration> &eye_configuration);
  virtual ~DeviceConfiguration () { }

  virtual std::string Identifier () const override { return m_identifier; }

  virtual float InterPupillaryDistance () const override { return m_inter_pupillary_distance; }
  virtual uint32_t EyeCount () const override { return m_eye_configuration.size(); }
  virtual uint32_t EyeRenderOrder (uint32_t eye_render_index) const override;
  virtual const OculusRift::EyeConfiguration &EyeConfiguration (uint32_t eye_index) const override;

private:

  const OculusRift::Context &m_context;
  const OculusRift::Device &m_device;

  std::string m_identifier;
  float m_inter_pupillary_distance; // avg for men is 65, for women is 62.  average of those is 63.5.
  std::vector<uint32_t> m_eye_render_order;
  std::vector<OculusRift::EyeConfiguration> m_eye_configuration;
};

} // end of namespace OculusRift
} // end of namespace Leap
