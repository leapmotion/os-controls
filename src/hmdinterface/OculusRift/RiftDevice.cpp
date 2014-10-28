#include "GLError.h"
#include "RiftDevice.h"
#include "RiftException.h"
#include "RiftPose.h"

#include <cstddef>
#include <iostream>
#include <typeinfo>

#include "OVR_CAPI_GL.h"

///////////////////////////////////////////////////////////////////////////////////////////////////


namespace OculusRift {

Device::Device (const OculusRift::Context& context)
  : m_context(&context)
  , m_device_configuration(nullptr)
  , m_hmd(nullptr)
  , m_is_debug(false)
  , m_FrameBuffer(0)
  , m_Texture(0)
  , m_RenderBuffer(0)
{ }

Device::~Device () {
  if (IsInitialized_()) {
    std::cerr << "Warning: Leap::OculusRift::Device instance was not Shutdown() before destruction.\n";
    Device::Shutdown();
  }
}

void Device::Initialize () {
  // Attempt to create the HMD device.
  // TODO: There's no documentation about the "index" parameter of ovrHmd_Create, if that
  // refers to one of potentially many Oculus Rift devices connected to the machine.  If
  // that is the case, then we should implement that.
  m_hmd = ovrHmd_Create(0);
  // If the creation failed, there is a fallback.
  if (!m_hmd) {
    m_hmd = ovrHmd_CreateDebug(ovrHmd_DK1);
    if (!m_hmd) {
      throw Exception("Oculus Rift initialization call failed.", m_context);
    }
    m_is_debug = true;
  } else {
    m_is_debug = false;
  }

  // Copied in from elsewhere
  ovrSizei recommended_tex_0_size = ovrHmd_GetFovTextureSize(m_hmd, ovrEye_Left, m_hmd->DefaultEyeFov[0], 1.0f);
  ovrSizei recommended_tex_1_size = ovrHmd_GetFovTextureSize(m_hmd, ovrEye_Right, m_hmd->DefaultEyeFov[1], 1.0f);

  ovrSizei render_target_size;
  render_target_size.w = recommended_tex_0_size.w + recommended_tex_1_size.w;
  render_target_size.h = std::max(recommended_tex_0_size.h, recommended_tex_1_size.h);

  glGenFramebuffers(1, &m_FrameBuffer);

  glGenTextures(1, &m_Texture);

  glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
  glBindTexture(GL_TEXTURE_2D, m_Texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_target_size.w, render_target_size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  if (false) {
    // TODO: enable mipmap for this one -- though is that actually useful? -- are we rendering
    // texel-to-pixel?  the answer is probably no, because there is a distortion mesh that is
    // used to do the barrel distortion.
    glGenerateMipmap(GL_TEXTURE_2D);
    // TODO: will need to re-generate mipmaps for the texture once it's rendered to.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    // No mipmap necessary
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

  glGenRenderbuffers(1, &m_RenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, render_target_size.w, render_target_size.h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    Shutdown();
    throw Exception("Creation of framebuffer for OculusRift::Device failed.", m_context);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ovrFovPort eye_fov[2] = { m_hmd->DefaultEyeFov[0], m_hmd->DefaultEyeFov[1] };

  m_EyeRenderViewport[0].Pos.x = 0;
  m_EyeRenderViewport[0].Pos.y = 0;
  m_EyeRenderViewport[0].Size.w = render_target_size.w / 2;
  m_EyeRenderViewport[0].Size.h = render_target_size.h;
  m_EyeRenderViewport[1].Pos.x = (render_target_size.w + 1) / 2;
  m_EyeRenderViewport[1].Pos.y = 0;
  m_EyeRenderViewport[1].Size = m_EyeRenderViewport[0].Size;

  m_EyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
  m_EyeTexture[0].OGL.Header.TextureSize = render_target_size;
  m_EyeTexture[0].OGL.Header.RenderViewport = m_EyeRenderViewport[0];
  m_EyeTexture[0].OGL.TexId = m_Texture;

  m_EyeTexture[1] = m_EyeTexture[0];
  m_EyeTexture[1].OGL.Header.RenderViewport = m_EyeRenderViewport[1];

  ovrGLConfig cfg;
  cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
  cfg.OGL.Header.RTSize.w = m_hmd->Resolution.w;
  cfg.OGL.Header.RTSize.h = m_hmd->Resolution.h;
  cfg.OGL.Header.Multisample = 1;

  if (!(m_hmd->HmdCaps & ovrHmdCap_ExtendDesktop)) {
    ovrHmd_AttachToWindow(m_hmd, m_Window, nullptr, nullptr);
  }

#if defined(OVR_OS_WIN32)
  cfg.OGL.Window = m_Window;
  cfg.OGL.DC = nullptr;
#elif defined(OVR_OS_LINUX)
  cfg.OGL.Disp = nullptr;
  cfg.OGL.Win = m_Window;
#endif

  ovrEyeRenderDesc EyeRenderDesc[2];
  ovrHmd_ConfigureRendering(
    m_hmd,
    &cfg.Config,
    ovrDistortionCap_Chromatic|ovrDistortionCap_Vignette|ovrDistortionCap_TimeWarp|ovrDistortionCap_Overdrive,
    eye_fov,
    EyeRenderDesc);

  // Internally, the above line calls glewInit(), which generates a GL_INVALID_ENUM error inside of it.
  // We will make a glGetError() call to clear out the phony error; otherwise the next gl function we
  // call will appear to fail. Raffi, I'm not sure if your glewInit() changes in develop resolve this.
  // If so, this might not be needed anymore.
  glGetError();

  ovrHmd_SetEnabledCaps(m_hmd, ovrHmdCap_LowPersistence|ovrHmdCap_DynamicPrediction);

  ovrHmd_ConfigureTracking(
    m_hmd,
    ovrTrackingCap_Orientation|ovrTrackingCap_MagYawCorrection|ovrTrackingCap_Position,
    0);

  //End Copied in from elsewhere
  std::string device_identifier("Oculus Rift"); // TODO: real string with model and version info, etc.
  uint32_t eye_count = ovrEye_Count; // TODO: real count read from the device (?)
  std::vector<uint32_t> eye_render_order;
  std::vector<OculusRift::EyeConfiguration> eye_configuration;
  for (uint32_t eye_render_index = 0; eye_render_index < eye_count; ++eye_render_index) {
    ovrEyeType eye_type = m_hmd->EyeRenderOrder[eye_render_index];
    eye_render_order.emplace_back(eye_type);
    eye_configuration.emplace_back(eye_type, m_hmd->DefaultEyeFov[eye_render_index], EyeRenderDesc[eye_render_index]);
  }

  m_device_configuration =
    new DeviceConfiguration(
      *m_context,
      *this,
      device_identifier,
      m_hmd->Resolution.w,
      m_hmd->Resolution.h,
      m_hmd->WindowsPos.x,
      m_hmd->WindowsPos.y,
      63.5f, // inter-pupillary distance
      eye_render_order,
      eye_configuration);

  assert(IsInitialized() && "programmer error -- a post-condition of Initialize() should be that IsInitialized() returns true.");
}

bool Device::IsInitialized () const {
  bool is_initialized = IsInitialized_();
  if (is_initialized) {
    assert(m_context != nullptr && 
           m_device_configuration != nullptr &&
           m_hmd != nullptr &&
           m_FrameBuffer != 0 &&
           m_Texture != 0 &&
           m_RenderBuffer != 0 && "Resources were initialized inconsistently");
  } else {
    assert(m_context == nullptr &&
           m_device_configuration == nullptr &&
           m_hmd == nullptr &&
           m_FrameBuffer == 0 &&
           m_Texture == 0 &&
           m_RenderBuffer == 0 && "Resources were shutdown inconsistently");
  }
  return IsInitialized_();
}

void Device::Shutdown () {
  if (!IsInitialized()) {
    return; // Nothing to do.
  }

  m_context = nullptr; // We never owned the context to begin with.

  delete m_device_configuration;
  m_device_configuration = nullptr;

  assert(m_hmd != nullptr);
  ovrHmd_Destroy(m_hmd);
  m_hmd = nullptr;

  // A delete call will ignore any handle that is 0, which is the sentinel value for such handles.
  glDeleteFramebuffers(1, &m_FrameBuffer);
  glDeleteTextures(1, &m_Texture);
  glDeleteRenderbuffers(1, &m_RenderBuffer);
  // Reset the handles to the sentinel value.
  m_FrameBuffer = m_Texture = m_RenderBuffer = 0;

  assert(!IsInitialized() && "programmer error -- a post-condition of Shutdown() should be that IsInitialized() returns false.");
}

void Device::DismissHealthWarning () const {
  if (!IsInitialized()) {
    throw Exception("Can't dismiss the health warning on a Device that hasn't been Initialize()'d.");
  }
  ovrHmd_DismissHSWDisplay(m_hmd);
}

const OculusRift::Context &Device::Context () const {
  if (!IsInitialized()) {
    throw Exception("Can't retrieve the Context from a Device that hasn't been Initialize()'d.");
  }
  assert(m_context != nullptr);
  return *m_context;
}

const OculusRift::DeviceConfiguration &Device::Configuration () const {
  if (!IsInitialized())
    throw Exception("Call to Device::Configuration is undefined unless Device::IsInitialized() returns true.", m_context, this);
  assert(m_device_configuration != nullptr);
  return *m_device_configuration;
}

void Device::BeginFrame () {
  ovrFrameTiming frameTiming = ovrHmd_BeginFrame(m_hmd, 0);

  for (uint32_t eye_index = 0; eye_index < ovrEye_Count; ++eye_index) {
    ovrEyeType eye = m_hmd->EyeRenderOrder[eye_index];
    ovrPosef &EyePosef = m_CachedEyeRenderPoseForEndFrame[eye_index];
    EyePosef = ovrHmd_GetEyePose(m_hmd, eye);
    static_assert(sizeof(ovrPosef) == sizeof(OVR::Pose<float>), "The conversion between ovrPosef and OVR::Pose<float> is done under the assumption of direct memory mapping of members.");
    static_assert(offsetof(ovrPosef,Orientation) == offsetof(OVR::Pose<float>,Rotation), "The conversion between ovrPosef and OVR::Pose<float> is done under the assumption of direct memory mapping of members.");
    static_assert(offsetof(ovrPosef,Position) == offsetof(OVR::Pose<float>,Translation), "The conversion between ovrPosef and OVR::Pose<float> is done under the assumption of direct memory mapping of members.");
    m_EyeRenderPose[eye] = OVR::Pose<double>(*reinterpret_cast<OVR::Pose<float> *>(&EyePosef));
  }

  glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
  GLThrowUponError("glBindFramebuffer");
}

std::shared_ptr<Hmd::IPose> Device::EyePose (uint32_t eye_index) const {
  return std::make_shared<OculusRift::Pose>(m_EyeRenderPose[eye_index]);
}

void Device::BeginRenderingEye (uint32_t eye_index) const {
  assert(eye_index < ovrEye_Count && "eye_index out of range."); // TODO: use Configuration-based eye count
  const ovrRecti &rect = m_EyeRenderViewport[eye_index];
  glViewport(rect.Pos.x, rect.Pos.y, rect.Size.w, rect.Size.h);
}

void Device::EndRenderingEye (uint32_t eye_index) const {

}

void Device::EndFrame () {
  // Note that ovrGLTexture is a union of ovrTexture and ovrGLTextureData, so this is using
  // m_EyeTexture as type ovrTexture[2].
  ovrHmd_EndFrame(m_hmd, m_CachedEyeRenderPoseForEndFrame, &m_EyeTexture[0].Texture);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  DismissHealthWarning();
}

} // end of namespace OculusRift
