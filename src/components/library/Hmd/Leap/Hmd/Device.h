#pragma once

#include "Leap/Hmd/Pose.h"
#include <memory>

namespace Leap {
namespace Hmd {

class Context;
class DeviceConfiguration;

class Device {
public:

  virtual ~Device () { }

  /// @brief Initialize this Device object.
  /// @details If this method call succeeds, then the initialization has succeeded.  Errors are
  /// indicated by throwing an exception that is a subclass of Hmd::Exception.  If this method
  /// call succeeds, then IsInitialized() must return true until Shutdown() is called, at which
  /// point, IsInitialized() must return false.
  virtual void Initialize (Hmd::Context &context) = 0;
  /// @brief Returns true if and only if this object has been successfully initialized.
  /// @details This method must return false before Initialize() is successfully called,
  /// and must return false after Shutdown() is called.
  virtual bool IsInitialized () const = 0;
  /// @brief Perform shutdown routines for this device.
  /// @details This method must be called explicitly and must be called before destruction.  If
  /// IsInitialized() returns false before calling this method, then this call should have no
  /// effect.  In any case, IsInitialized() must return false after this method is called.
  virtual void Shutdown () = 0;

  /// @brief Returns the Context used to Initialize this Device.
  /// @details If IsInitialized is false, this method will throw an exception that is
  /// a subclass of Hmd::Exception.
  virtual const Hmd::Context &Context () const = 0;
  /// @brief Returns the DeviceConfiguration which reflects the actual configuration state of the device.
  /// @details If IsInitialized is false, this method will throw an exception that is
  /// a subclass of Hmd::Exception.
  virtual const DeviceConfiguration &ActualConfiguration () const = 0;

  virtual void BeginFrame () = 0;
  /// @brief Returns the Pose of the given eye for the current frame.
  /// @details This method should only be called between BeginFrame() and EndFrame().
  virtual std::shared_ptr<Pose> EyePose (uint32_t eye_index) const = 0;
  // /// @brief Returns the current sensor readings of the HMD.
  // /// @details This method should only be called between BeginFrame() and EndFrame().
  // virtual std::shared_ptr<SensorData> SensorReadings () const = 0;
  virtual void BeginRenderingEye (uint32_t eye_index) const = 0;
  virtual void EndRenderingEye (uint32_t eye_index) const = 0;
  virtual void EndFrame () = 0;
};

} // end of namespace Hmd
} // end of namespace Leap
