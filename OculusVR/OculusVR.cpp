#include "OculusVR.h"
#include <algorithm>

bool OculusVR::Init() {
  m_Debug = false;

  ovr_Initialize();

  m_HMD = ovrHmd_Create(0);

  if (!m_HMD) {
    m_HMD = ovrHmd_CreateDebug(ovrHmd_DK1);
    if (!m_HMD) {
      return false;
    }
    m_Debug = true;
  }

  const int width = m_HMD->Resolution.w;
  const int height = m_HMD->Resolution.h;

  ovrSizei recommendedTex0Size = ovrHmd_GetFovTextureSize(m_HMD, ovrEye_Left, m_HMD->DefaultEyeFov[0], 1.0f);
  ovrSizei recommendedTex1Size = ovrHmd_GetFovTextureSize(m_HMD, ovrEye_Right, m_HMD->DefaultEyeFov[1], 1.0f);

  ovrSizei renderTargetSize;
  renderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
  renderTargetSize.h = std::max(recommendedTex0Size.h, recommendedTex1Size.h);

  FrameBufferObject::Format format;
  format.depth = true;
  m_HMDFbo = FrameBufferObject::Create(renderTargetSize.w, renderTargetSize.h, format);

  if (!m_HMDFbo) {
    Shutdown();
    return false;
  }

  ovrFovPort eyeFov[2] ={ m_HMD->DefaultEyeFov[0], m_HMD->DefaultEyeFov[1] };

  m_EyeRenderViewport[0].Pos.x = 0;
  m_EyeRenderViewport[0].Pos.y = 0;
  m_EyeRenderViewport[0].Size.w = renderTargetSize.w / 2;
  m_EyeRenderViewport[0].Size.h = renderTargetSize.h;
  m_EyeRenderViewport[1].Pos.x = (renderTargetSize.w + 1) / 2;
  m_EyeRenderViewport[1].Pos.y = 0;
  m_EyeRenderViewport[1].Size = m_EyeRenderViewport[0].Size;

  m_EyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
  m_EyeTexture[0].OGL.Header.TextureSize = renderTargetSize;
  m_EyeTexture[0].OGL.Header.RenderViewport = m_EyeRenderViewport[0];
  m_EyeTexture[0].OGL.TexId = m_HMDFbo->ColorTexture()->Id();

  m_EyeTexture[1] = m_EyeTexture[0];
  m_EyeTexture[1].OGL.Header.RenderViewport = m_EyeRenderViewport[1];

  ovrGLConfig cfg;
  cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
  cfg.OGL.Header.RTSize.w = m_HMD->Resolution.w;
  cfg.OGL.Header.RTSize.h = m_HMD->Resolution.h;
  cfg.OGL.Header.Multisample = 1;

#if 0 // TODO: pass in native window here

#if defined(OVR_OS_WIN32)
  if (!(m_HMD->HmdCaps & ovrHmdCap_ExtendDesktop))
    ovrHmd_AttachToWindow(m_HMD, info.info.win.window, NULL, NULL);

  cfg.OGL.Window = info.info.win.window;
  cfg.OGL.DC = NULL;
#elif defined(OVR_OS_LINUX)
  cfg.OGL.Disp = info.info.x11.display;
  cfg.OGL.Win = info.info.x11.window;
#endif

#endif
 
  ovrHmd_ConfigureRendering(m_HMD, &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive, eyeFov, m_EyeRenderDesc);

  ovrHmd_SetEnabledCaps(m_HMD, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

  ovrHmd_ConfigureTracking(m_HMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);

  return true;
}

OculusVR::~OculusVR() {
  delete m_HMDFbo;
}

void OculusVR::BeginFrame() {
  ovrFrameTiming frameTiming = ovrHmd_BeginFrame(m_HMD, 0);

  m_HMDFbo->Bind();

  for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++) {
    ovrEyeType eye = m_HMD->EyeRenderOrder[eyeIndex];
    m_EyeRenderPose[eye] = ovrHmd_GetEyePose(m_HMD, eye);
    m_EyeProjection[eye] = ovrMatrix4f_Projection(m_EyeRenderDesc[eye].Fov, 0.1f, 10000.0f, true);
    m_EyeTranslation[eye] = m_EyeRenderDesc[eye].ViewAdjust;
    m_EyeRotation[eye] = m_EyeRenderPose[eye].Orientation;
  }
}

void OculusVR::EndFrame() {
  m_HMDFbo->Unbind();
  ovrHmd_EndFrame(m_HMD, m_EyeRenderPose, &m_EyeTexture[0].Texture);
}

void OculusVR::DismissHealthWarning() {
  ovrHmd_DismissHSWDisplay(m_HMD);
}

void OculusVR::Shutdown() {
  ovrHmd_Destroy(m_HMD);
  ovr_Shutdown();
}
