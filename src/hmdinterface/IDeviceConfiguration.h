#pragma once

#include <cstdint>
#include <string>

namespace Hmd {

class IEyeConfiguration;

class IDeviceConfiguration {
public:

  virtual ~IDeviceConfiguration () { }

  virtual std::string Identifier () const = 0;

  virtual size_t DisplayWidth () const = 0;
  virtual size_t DisplayHeight () const = 0;
  virtual int WindowPositionX () const = 0;
  virtual int WindowPositionY () const = 0;
  virtual float InterPupillaryDistance () const = 0;
  virtual uint32_t EyeCount () const = 0;
  virtual uint32_t EyeRenderOrder (uint32_t eye_render_order_index) const = 0;
  virtual const Hmd::IEyeConfiguration &EyeConfiguration (uint32_t eye_index) const = 0;
};

} // end of namespace Hmd
