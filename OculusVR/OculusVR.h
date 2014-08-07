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


class OculusVR {

public:

  bool Init();
  ~OculusVR();

  void BeginFrame();
  void EndFrame();

  void DismissHealthWarning();

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
