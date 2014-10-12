#pragma once

#include "Leap/Hmd/Pose.h"
#include <memory>

namespace Leap {
namespace Hmd {

class DeviceConfiguration;

class Device {
public:

  virtual ~Device () { }

  virtual void Initialize (const DeviceConfiguration &requested_configuration) = 0;
  virtual bool IsInitialized () = 0;
  virtual void Shutdown () = 0;

  virtual const DeviceConfiguration &ActualConfiguration () const = 0;
  virtual std::shared_ptr<Pose> EyePose (uint32_t eye_index) const = 0;
  virtual std::shared_ptr<SensorData> SensorReadings () const = 0;

  virtual void BeginFrame () const = 0; // TODO: should this be non-const?
  virtual void EndFrame () const = 0; // TODO: should this be non-const?
};

} // end of namespace Hmd
} // end of namespace Leap
