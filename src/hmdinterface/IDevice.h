#pragma once

#include "IPose.h"
#include "osinterface/OSWindowHandle.h"

#include <memory>

#include <autowiring/ContextMember.h>

namespace Hmd {

class IContext;
class IDeviceConfiguration;

#if 0
TODO LATER

enum class Capability { NOT_REQUESTED, REQUESTED_IF_PRESENT, REQUIRED };

/// @brief Simple container for specifing device initialization parameters for use in Device::Initialize.
/// @details The default constructor sets the parameters to reasonable defaults.
/// There are several types of parameters:
/// - Requests for capabilities; the default value is Capability::NOT_REQUESTED.  The value
///   Capability::REQUESTED_IF_PRESENT indicates that the capability should be used if present
///   but that the lack of said capability is not an error.  The value Capability::REQUIRED
///   indicates that the capability must be present and that it is an initialization error
///   otherwise.
/// - Configuration values; numerical values.
///   * InterPupillary Distance (IPD).
///   * Eye FOVs.
///   * Pixel density at eye view centers.
class DeviceInitializationParameters {
public:

  /// @brief Constructs an object of this type with reasonable defaults.
  /// @details Initializes the parameters to reasonable defaults, notably setting all
  /// capability requests to Capability::NOT_REQUESTED.
  DeviceInitializationParameters ();



private:

  bool m_RequestLowPersistence;
  bool m_RequestDynamicPrediction;
  bool m_RequestNoVSync;
};
#endif

class IDevice : 
  public ContextMember
{
public:

  virtual ~IDevice () { }

  static IDevice* New();

  /// @brief Initialize this Device object.
  /// @details If this method call succeeds, then the initialization has succeeded.  Errors are
  /// indicated by throwing an exception that is a subclass of Hmd::IException.  If this method
  /// call succeeds, then IsInitialized() must return true until Shutdown() is called, at which
  /// point, IsInitialized() must return false.
  virtual void Initialize () = 0;
  /// @brief Returns true if and only if this object has been successfully initialized.
  /// @details This method must return false before Initialize() is successfully called,
  /// and must return false after Shutdown() is called.
  virtual bool IsInitialized () const = 0;
  /// @brief Perform shutdown routines for this device.
  /// @details This method must be called explicitly and must be called before destruction.  If
  /// IsInitialized() returns false before calling this method, then this call should have no
  /// effect.  In any case, IsInitialized() must return false after this method is called.
  virtual void Shutdown () = 0;

  /// @brief Associate an os-window where rendering can happen
  /// @details This method should generally be called before Initialize, although that
  /// requirement may need to be relaxed as we support more HMD types.  This setting may
  /// be moved into an initalizer struct at a later time.
  virtual void SetWindow(const WindowHandle& window) = 0;

  /// @brief Returns the Context used to create this Device.
  /// @details If IsInitialized is false, this method will throw an exception that is
  /// a subclass of Hmd::IException.
  virtual const Hmd::IContext &Context () const = 0;
  /// @brief Returns the DeviceConfiguration which reflects the actual configuration state of the device.
  /// @details If IsInitialized is false, this method will throw an exception that is
  /// a subclass of Hmd::IException.
  virtual const IDeviceConfiguration &Configuration () const = 0;

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  /// @brief Returns the Pose of the given eye for the current frame.
  /// @details This method should only be called between BeginFrame() and EndFrame().
  virtual std::shared_ptr<IPose> EyePose (uint32_t eye_index) const = 0;
  
  // /// @brief Returns the current sensor readings of the HMD.
  // /// @details This method should only be called between BeginFrame() and EndFrame().
  // virtual std::shared_ptr<SensorData> SensorReadings () const = 0; // TODO
  virtual void BeginRenderingEye (uint32_t eye_index) const = 0;
  virtual void EndRenderingEye (uint32_t eye_index) const = 0;
  
  virtual void DismissHealthWarning() const = 0;
  
};

} // end of namespace Hmd
