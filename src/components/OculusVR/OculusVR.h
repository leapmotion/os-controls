#pragma once

#include "EigenTypes.h"
#include "OVR.h"
#include "OVR_Kernel.h"
#include <gl_glext_glu.h>

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
  bool isDebug();
  
  int GetHMDWidth();
  
  int GetHMDHeight();
  
  void InitGlew();

  void InitHMD();
  bool Init();
  void Destroy();

  void BeginFrame();
  void EndFrame();

  void DismissHealthWarning();
  
  const ovrRecti& EyeViewport(int eye) const {
    return m_EyeRenderViewport[eye];
  }

  EigenTypes::Matrix4x4f EyeView(int eye) const {
    return EigenTypes::Matrix4x4f(&m_EyeView[eye].Transposed().M[0][0]);
  }

  EigenTypes::Matrix4x4f EyeProjection(int eye) const {
    return EigenTypes::Matrix4x4f(&m_EyeProjection[eye].Transposed().M[0][0]);
  }

  EigenTypes::Vector3f EyePosition(int eye) const {
    return EigenTypes::Vector3f(m_EyePosition[eye].x, m_EyePosition[eye].y, m_EyePosition[eye].z);
  }

  EigenTypes::Matrix4x4f EyeRotation(int eye) const {
    return EigenTypes::Matrix4x4f(&m_EyeRotation[eye].Transposed().M[0][0]);
  }

  ovrHmd& GetHMD() {
    return m_HMD;
  }
  
  void GetFramebufferStatus(GLenum status);

#if _WIN32
  void SetHWND(const HWND& hwnd) {
    m_HWND = hwnd;
  }
#endif

private:

  void Shutdown();

  ovrHmd m_HMD = NULL;
  bool m_Debug;
  
  int m_width;
  int m_height;

  GLuint m_FrameBuffer;
  GLuint m_Texture;
  GLuint m_RenderBuffer;

  ovrRecti m_EyeRenderViewport[2];
  ovrGLTexture m_EyeTexture[2];
  ovrPosef m_EyeRenderPose[2];
  ovrEyeRenderDesc m_EyeRenderDesc[2];

  OVR::Matrix4f m_EyeProjection[2];
  OVR::Matrix4f m_EyeView[2];

  OVR::Vector3f m_EyePosition[2];
  OVR::Matrix4f m_EyeRotation[2];

#if _WIN32
  HWND m_HWND;
#endif

};
