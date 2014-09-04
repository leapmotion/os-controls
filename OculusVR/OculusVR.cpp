#include "OculusVR.h"
#include <algorithm>

extern "C" {
void ovrhmd_EnableHSWDisplaySDKRender(ovrHmd hmd, ovrBool enabled);
}

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

  glGenFramebuffers(1, &m_FrameBuffer);

  glGenTextures(1, &m_Texture);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
  glBindTexture(GL_TEXTURE_2D, m_Texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderTargetSize.w, renderTargetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture, 0);

  glGenRenderbuffers(1, &m_RenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderTargetSize.w, renderTargetSize.h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    Shutdown();
    return false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ovrFovPort eyeFov[2] = { m_HMD->DefaultEyeFov[0], m_HMD->DefaultEyeFov[1] };

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
  m_EyeTexture[0].OGL.TexId = m_Texture;

  m_EyeTexture[1] = m_EyeTexture[0];
  m_EyeTexture[1].OGL.Header.RenderViewport = m_EyeRenderViewport[1];

  ovrGLConfig cfg;
  cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
  cfg.OGL.Header.RTSize.w = m_HMD->Resolution.w;
  cfg.OGL.Header.RTSize.h = m_HMD->Resolution.h;
  cfg.OGL.Header.Multisample = 1;

#if _WIN32
  if (!(m_HMD->HmdCaps & ovrHmdCap_ExtendDesktop)) {
    ovrHmd_AttachToWindow(m_HMD, m_HWND, NULL, NULL);
  }

  cfg.OGL.Window = m_HWND;
  cfg.OGL.DC = NULL;
#endif
 
  ovrHmd_ConfigureRendering(m_HMD, &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive, eyeFov, m_EyeRenderDesc);

  // Internally, the above line calls glewInit(), which generates a GL_INVALID_ENUM error inside of it. We will make a
  // glGetError() call to clear out the phony error; otherwise the next gl function we call will appear to fail. Raffi, I'm 
  // not sure if your glewInit() changes in develop resolves this? If so, this might not be needed anymore.
  glGetError();

  ovrHmd_SetEnabledCaps(m_HMD, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

  ovrHmd_ConfigureTracking(m_HMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);

  return true;
}

OculusVR::~OculusVR() {
  glDeleteFramebuffers(1, &m_FrameBuffer);
  glDeleteTextures(1, &m_Texture);
  glDeleteRenderbuffers(1, &m_RenderBuffer);
}

void OculusVR::BeginFrame() {
  ovrFrameTiming frameTiming = ovrHmd_BeginFrame(m_HMD, 0);
  
  static OVR::Vector3f HeadPos(0.0f, 1.6f, -5.0f);
  HeadPos.y = ovrHmd_GetFloat(m_HMD, OVR_KEY_EYE_HEIGHT, HeadPos.y);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

  for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++) {
    ovrEyeType eye = m_HMD->EyeRenderOrder[eyeIndex];
    m_EyeRenderPose[eye] = ovrHmd_GetEyePose(m_HMD, eye);
    m_EyeProjection[eye] = ovrMatrix4f_Projection(m_EyeRenderDesc[eye].Fov, 0.1f, 10000.0f, true);

    const OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(0);
    const OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(m_EyeRenderPose[eye].Orientation);
    const OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
    const OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
    const OVR::Vector3f shiftedEyePos = HeadPos + rollPitchYaw.Transform(m_EyeRenderPose[eye].Position);
    const OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
    m_EyeView[eye] = OVR::Matrix4f::Translation(m_EyeRenderDesc[eye].ViewAdjust) * view;
  }
}

void OculusVR::EndFrame() {
  ovrHmd_EndFrame(m_HMD, m_EyeRenderPose, &m_EyeTexture[0].Texture);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OculusVR::DismissHealthWarning() {
  ovrHmd_DismissHSWDisplay(m_HMD);
  ovrhmd_EnableHSWDisplaySDKRender(m_HMD, false);
}

void OculusVR::Shutdown() {
  ovrHmd_Destroy(m_HMD);
  ovr_Shutdown();
}
