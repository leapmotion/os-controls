#pragma once

#include "IDevice.h"
#include "Leap/GL/GLHeaders.h" // TODO: figure out how not to include this here.
#include "OculusRift/RiftContext.h"
#include "OculusRift/RiftDeviceConfiguration.h"

#include "OVR.h"
#include "OVR_Kernel.h"
#include "OVR_CAPI_GL.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Leap::GL;

namespace OculusRift {

class Device : public Hmd::IDevice {
public:
  Device(const OculusRift::Context &context);
  virtual ~Device ();

  // If this is to be called, it must be done before Initialize.
  // NOTE: This is from OculusVR, and will likely not stay -- it will be abstracted into
  // a DeviceInitializationParameters interface, of which there will be an OculusRift
  // implementation.
  void SetWindow (const WindowHandle &window_handle) override { m_Window = window_handle; }

  // OpenGL must be initialized before this call is made, as this creates certain GL resources.
  virtual void Initialize () override;
  virtual bool IsInitialized () const override;
  virtual void Shutdown () override;

  void DismissHealthWarning () const override;

  /// @brief Returns the Context used to Initialize this Device.
  /// @details If IsInitialized is false, this method will throw an exception that is
  /// a subclass of Hmd::IException.
  /// @note The return type is a subclass of the Hmd::IDevice interface method's return
  /// type Hmd::IContext.
  virtual const OculusRift::Context &Context () const override;
  /// @brief Returns the DeviceConfiguration which reflects the actual configuration state of the device.
  /// @details If IsInitialized is false, this method will throw an exception that is
  /// a subclass of Hmd::IException.
  /// @note The return type is a subclass of the Hmd::IDevice interface method's return
  /// type Hmd::IDeviceConfiguration.
  virtual const OculusRift::DeviceConfiguration &Configuration () const override;

  virtual void BeginFrame () override;
  // Only accessible between BeginFrame and EndFrame
  virtual std::shared_ptr<Hmd::IPose> EyePose (uint32_t eye_index) const override;
  // virtual std::shared_ptr<SensorData> SensorReadings () const override;
  virtual void BeginRenderingEye (uint32_t eye_index) const override;
  virtual void EndRenderingEye (uint32_t eye_index) const override;
  virtual void EndFrame () override;

private:

  // This is the implementation of IsInitialized (useful as a non-virtual method).
  bool IsInitialized_ () const { return m_hmd != nullptr; }

  const OculusRift::Context *m_context;
  const OculusRift::DeviceConfiguration *m_device_configuration;
  
  ovrHmd m_hmd;
  bool m_is_debug;
  GLuint m_FrameBuffer;
  GLuint m_Texture;
  GLuint m_RenderBuffer;

  ovrRecti m_EyeRenderViewport[2];
  ovrGLTexture m_EyeTexture[2];
  // ovrHmd_EndFrame requires that the eye poses be passed in.
  ovrPosef m_CachedEyeRenderPoseForEndFrame[2];
  // Store the eye poses as OVR::Pose<double> because the relevant C++ classes
  // are actually useful instead of the lame C struct ovrPosef.
  OVR::Pose<double> m_EyeRenderPose[2];

  OVR::Matrix4f m_EyeProjection[2];
  OVR::Matrix4f m_EyeView[2];

  OVR::Vector3f m_EyePosition[2];
  OVR::Matrix4f m_EyeRotation[2];

  WindowHandle m_Window;
};

} // end of namespace OculusRift
