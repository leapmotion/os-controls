#pragma once

#include "gl_glext_glu.h" // TODO: figure out how not to include this here.
#include "IDevice.h"
#include "OculusRift/RiftContext.h"
#include "OculusRift/RiftDeviceConfiguration.h"

#include "OVR.h"
#include "OVR_Kernel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// this is code copied from an apparently internal .h file in LibOVR (OVR_CAPI_GL.h),
// and should probably be handled differently

/// Used to pass GL eye texture data to ovrHmd_EndFrame.
typedef struct ovrGLTextureData_s
{
  /// General device settings.
  ovrTextureHeader Header;
  /// The OpenGL name for this texture.
  GLuint           TexId;
} ovrGLTextureData;

/// Contains OpenGL-specific texture information.
typedef union ovrGLTexture_s
{
  /// General device settings.
  ovrTexture       Texture;
  /// OpenGL-specific settings.
  ovrGLTextureData OGL;
} ovrGLTexture;

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Leap {
namespace OculusRift {

class Device : public Hmd::IDevice {
public:

  Device ();
  virtual ~Device ();

#if defined(OVR_OS_WIN32)
  typedef HWND WindowHandle;
#elif defined(OVR_OS_MAC)
  typedef void* WindowHandle; // NSWindow
#elif defined(OVR_OS_LINUX)
  typedef Window WindowHandle;
#endif

  // If this is to be called, it must be done before Initialize.
  // NOTE: This is from OculusVR, and will likely not stay -- it will be abstracted into
  // a DeviceInitializationParameters interface, of which there will be an OculusRift
  // implementation.
  void SetWindow (const WindowHandle &window_handle) { m_Window = window_handle; }

  // OpenGL must be initialized before this call is made, as this creates certain GL resources.
  virtual void Initialize (Hmd::IContext &context) override;
  virtual bool IsInitialized () const override;
  virtual void Shutdown () override;

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
  virtual const OculusRift::DeviceConfiguration &ActualConfiguration () const override;

  virtual void BeginFrame () override;
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
  ovrEyeRenderDesc m_EyeRenderDesc[2];

  OVR::Matrix4f m_EyeProjection[2];
  OVR::Matrix4f m_EyeView[2];

  OVR::Vector3f m_EyePosition[2];
  OVR::Matrix4f m_EyeRotation[2];

  WindowHandle m_Window;
};

} // end of namespace OculusRift
} // end of namespace Leap
