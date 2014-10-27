#pragma once

#include "IDeviceConfiguration.h"
#include "OculusRift/RiftEyeConfiguration.h"

#include <cstdint>
#include <string>
#include <vector>

namespace OculusRift {

class Context;
class Device;

class DeviceConfiguration : public Hmd::IDeviceConfiguration {
public:

  DeviceConfiguration (
    const OculusRift::Context &context,
    const OculusRift::Device &device,
    const std::string &identifier,
    size_t display_width,
    size_t display_height,
    int window_position_x,
    int window_position_y,
    float inter_pupillary_distance,
    const std::vector<uint32_t> &eye_render_order,
    const std::vector<OculusRift::EyeConfiguration> &eye_configuration);
  virtual ~DeviceConfiguration () { }

  virtual std::string Identifier () const override { return m_identifier; }

  virtual size_t DisplayWidth () const override { return m_display_width; }
  virtual size_t DisplayHeight () const override { return m_display_height; }
  virtual int WindowPositionX () const override { return m_window_position_x; }
  virtual int WindowPositionY () const override { return m_window_position_y; }
  virtual float InterPupillaryDistance () const override { return m_inter_pupillary_distance; }
  virtual uint32_t EyeCount () const override { return m_eye_configuration.size(); }
  virtual uint32_t EyeRenderOrder (uint32_t eye_render_index) const override;
  virtual const OculusRift::EyeConfiguration &EyeConfiguration (uint32_t eye_index) const override;

private:

  const OculusRift::Context &m_context;
  const OculusRift::Device &m_device;

  std::string m_identifier;
  size_t m_display_width;
  size_t m_display_height;
  int m_window_position_x;
  int m_window_position_y;
  float m_inter_pupillary_distance; // avg for men is 65, for women is 62.  average of those is 63.5.
  std::vector<uint32_t> m_eye_render_order;
  std::vector<OculusRift::EyeConfiguration> m_eye_configuration;
};

} // end of namespace OculusRift
