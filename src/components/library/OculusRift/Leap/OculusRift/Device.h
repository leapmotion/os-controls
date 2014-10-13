#pragma once

#include "gl_glext_glu.h"
#include "Leap/Hmd/Device.h"
#include "Leap/OculusRift/Context.h"
#include "Leap/OculusRift/DeviceConfiguration.h"
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

class Device : public Hmd::Device {
public:

  Device ();
  virtual ~Device ();

  // OpenGL must be initialized before this call is made, as this creates certain GL resources.
  virtual void Initialize (Hmd::Context &context, const Hmd::DeviceConfiguration &requested_configuration) override;
  virtual bool IsInitialized () override;
  virtual void Shutdown () override;

  // The scoping is to make it completely obvious that the return type has been overridden
  // to return the OculusRift-specific DeviceConfiguration implementation.
  virtual const OculusRift::DeviceConfiguration &ActualConfiguration () const override;
  virtual std::shared_ptr<Hmd::Pose> EyePose (uint32_t eye_index) const override;
  // virtual std::shared_ptr<SensorData> SensorReadings () const override;

  virtual void BeginFrame () override;
  virtual void EndFrame () override;

private:

  bool IsInitialized_Implementation () { return m_hmd != nullptr; }

  const OculusRift::Context *m_context;
  ovrHmd m_hmd;
  bool m_is_debug;
  GLuint m_FrameBuffer;
  GLuint m_Texture;
  GLuint m_RenderBuffer;
  OculusRift::DeviceConfiguration m_configuration;

  ovrRecti m_EyeRenderViewport[2];
  ovrGLTexture m_EyeTexture[2];
  ovrPosef m_EyeRenderPose[2];
  ovrEyeRenderDesc m_EyeRenderDesc[2];

  OVR::Matrix4f m_EyeProjection[2];
  OVR::Matrix4f m_EyeView[2];

  OVR::Vector3f m_EyePosition[2];
  OVR::Matrix4f m_EyeRotation[2];

#if defined(OVR_OS_WIN32)
  typedef HWND WindowHandle;
#elif defined(OVR_OS_MAC)
  typedef void* WindowHandle; // NSWindow
#elif defined(OVR_OS_LINUX)
  typedef Window WindowHandle;
#endif

  WindowHandle m_Window;
};

} // end of namespace OculusRift
} // end of namespace Leap
