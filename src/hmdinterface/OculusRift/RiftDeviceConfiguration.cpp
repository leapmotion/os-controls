#include "RiftDeviceConfiguration.h"
#include "RiftException.h"

#include <cassert>

namespace Leap {
namespace OculusRift {

DeviceConfiguration::DeviceConfiguration (
  const OculusRift::Context &context,
  const OculusRift::Device &device,
  const std::string &identifier,
  float inter_pupillary_distance,
  const std::vector<uint32_t> &eye_render_order,
  const std::vector<OculusRift::EyeConfiguration> &eye_configuration)
  : m_context(context)
  , m_device(device)
  , m_identifier(identifier)
  , m_inter_pupillary_distance(inter_pupillary_distance)
  , m_eye_render_order(eye_render_order)
  , m_eye_configuration(eye_configuration)
{
  std::string error_string;
  if (m_identifier.empty())
    error_string += " must specify a nonempty identifier;";
  if (m_inter_pupillary_distance < 0.0f)
    error_string += " must specify a nonnegative inter-pupillary distance;";
  if (eye_render_order.size() != eye_configuration.size())
    error_string += " eye_render_order and eye_configuration must have the same number of elements;";
  for (auto i : m_eye_render_order)
    if (i >= m_eye_render_order.size()) {
      error_string += " eye_render_order contains eye indices that are out of range;";
    }

  if (!error_string.empty())
    throw Exception("Leap::OculusRift::DeviceConfiguration():" + error_string, &m_context, &m_device);
}

uint32_t DeviceConfiguration::EyeRenderOrder (uint32_t eye_render_index) const {
  if (eye_render_index >= m_eye_render_order.size())
    throw Exception("Leap::OculusRift::EyeRenderOrder(): eye_render_index out of range.", &m_context, &m_device);
  return m_eye_render_order[eye_render_index];
}

const OculusRift::EyeConfiguration &DeviceConfiguration::EyeConfiguration (uint32_t eye_index) const {
  if (eye_index >= m_eye_configuration.size())
    throw Exception("Leap::OculusRift::EyeConfiguration(): eye_index out of range.", &m_context, &m_device);
  return m_eye_configuration[eye_index];
}

} // end of namespace OculusRift
} // end of namespace Leap

