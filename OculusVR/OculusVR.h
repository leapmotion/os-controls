#pragma once

#include "EigenTypes.h"
#include "OVR.h"
#include "GLTexture2.h"
#include "FrameBufferObject.h"


/// Used to configure slave GL rendering (i.e. for devices created externally).
typedef struct ovrGLConfigData_s
{
  /// General device settings.
  ovrRenderAPIConfigHeader Header;

#if defined(OVR_OS_WIN32)
  /// The optional window handle. If unset, rendering will use the current window.
  HWND Window;
  /// The optional device context. If unset, rendering will use a new context.
  HDC  DC;
#elif defined(OVR_OS_LINUX)
  /// The optional display. If unset, rendering will use the current display.
  _XDisplay* Disp;
  /// The optional window. If unset, rendering will use the current window.
  Window     Win;
#endif
} ovrGLConfigData;

/// Contains OpenGL-specific rendering information.
union ovrGLConfig
{
  /// General device settings.
  ovrRenderAPIConfig Config;
  /// OpenGL-specific settings.
  ovrGLConfigData    OGL;
};

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


// Provides an interface for retrieving tracking data and correcting distortion for an Oculus VR headset.
// Instructions for use:
// 1) Create an instance of OculusVR in your app
// 2) Call Init() and verify it returns true
// 3) Inside your app's main render loop:
//   a) Call BeginFrame()
//   b) Retrieve the eye viewport information with EyeViewport and call glViewport
//   c) Retrieve the eye transform using EyeProjection, EyeTranslation, and EyeRotation
//   d) Set up the OpenGL projection and modelview matrices appropriately
//   e) Render your geometry
//   f) Call EndFrame()
class OculusVR {

public:

  bool Init();
  ~OculusVR();

  void BeginFrame();
  void EndFrame();

  void DismissHealthWarning();
  
  const ovrRecti& EyeViewport(int eye) const {
    return m_EyeRenderViewport[eye];
  }

  const ovrMatrix4f& EyeProjection(int eye) const {
    return m_EyeProjection[eye];
  }

  const ovrVector3f& EyeTranslation(int eye) const {
    return m_EyeTranslation[eye];
  }

  const ovrQuatf& EyeRotation(int eye) const {
    return m_EyeRotation[eye];
  }

private:

  void Shutdown();

  ovrHmd m_HMD;
  bool m_Debug;

  FrameBufferObject* m_HMDFbo;
  ovrRecti m_EyeRenderViewport[2];
  ovrGLTexture m_EyeTexture[2];
  ovrPosef m_EyeRenderPose[2];
  ovrEyeRenderDesc m_EyeRenderDesc[2];

  ovrMatrix4f m_EyeProjection[2];
  ovrVector3f m_EyeTranslation[2];
  ovrQuatf m_EyeRotation[2];

};
