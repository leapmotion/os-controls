#include "GLBuffer.h"
#include "OculusVR.h"
#include "Globals.h"

#define SDK_RENDER 0

bool OculusVR::Init() {
  ovr_Initialize();
  m_EyePos = OVR::Vector3f(0.0f, 0.0f, 0.0f);
  m_EyeYaw = 3.14159265f;

  m_HMD = ovrHmd_Create(0);
  if (!m_HMD) {
    return false;
  }

  ovrHmd_GetDesc(m_HMD, &m_HMDDesc);
  if (m_HMDDesc.DisplayDeviceName[0] == '\0') {
    //return false;
  }

  m_LeftTexSize = ovrHmd_GetFovTextureSize(m_HMD, ovrEye_Left, m_HMDDesc.DefaultEyeFov[0], 1.0f);
  m_RightTexSize = ovrHmd_GetFovTextureSize(m_HMD, ovrEye_Right, m_HMDDesc.DefaultEyeFov[0], 1.0f);

  m_DistortionCaps = ovrDistortion_Chromatic | ovrDistortion_TimeWarp;

  m_RenderTargetSize.w = m_LeftTexSize.w + m_RightTexSize.w;
  m_RenderTargetSize.h = std::max(m_LeftTexSize.h, m_RightTexSize.h);


  m_EyeRenderViewport[0].Pos = OVR::Vector2i(0,0);
  m_EyeRenderViewport[0].Size = OVR::Sizei(m_RenderTargetSize.w/2, m_RenderTargetSize.h);
  m_EyeRenderViewport[1].Pos = OVR::Vector2i((m_RenderTargetSize.w+1)/2, 0);
  m_EyeRenderViewport[1].Size = m_EyeRenderViewport[0].Size;

  ci::gl::Fbo::Format format;
  format.enableDepthBuffer();
  format.setSamples(16);
  m_HMDFbo = ci::gl::Fbo(m_RenderTargetSize.w, m_RenderTargetSize.h, format);
  std::cout << "Init FBO size: " << m_HMDFbo.getSize() << std::endl;
#if SDK_RENDER

  m_EyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
  m_EyeTexture[0].OGL.Header.TextureSize = m_RenderTargetSize;
  m_EyeTexture[0].OGL.Header.RenderViewport = m_EyeRenderViewport[0];
  m_EyeTexture[0].OGL.TexId = m_HMDFbo.getTexture().getId();

  m_EyeTexture[1] = m_EyeTexture[0];
  m_EyeTexture[1].OGL.Header.RenderViewport = m_EyeRenderViewport[1];

  ovrGLConfig cfg;
  cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
  cfg.OGL.Header.RTSize = m_RenderTargetSize;
  cfg.OGL.Header.Multisample = 1;
  cfg.OGL.Window = m_HWND;

  if (!ovrHmd_ConfigureRendering(m_HMD, &cfg.Config, distortionCaps, m_HMDDesc.DefaultEyeFov, m_EyeRenderDesc)) {
    return false;
  }
#else
  m_EyeRenderDesc[0] = ovrHmd_GetRenderDesc(m_HMD, ovrEye_Left, m_HMDDesc.DefaultEyeFov[0]);
  m_EyeRenderDesc[1] = ovrHmd_GetRenderDesc(m_HMD, ovrEye_Right, m_HMDDesc.DefaultEyeFov[1]);

  // Create our own distortion mesh and shaders
  //DistortionMeshInit(distortionCaps, m_HMD, m_EyeRenderDesc, m_RenderTargetSize, m_EyeRenderViewport, pRender);
  DistortionMeshInit();
#endif

  ovrHmd_SetEnabledCaps(m_HMD, ovrHmdCap_LowPersistence | ovrHmdCap_LatencyTest);
  ovrHmd_StartSensor(m_HMD, ovrSensorCap_Orientation | ovrSensorCap_YawCorrection | ovrSensorCap_Position, 0);

  m_EyePos = OVR::Vector3f(0.0f, 0.0f, 0.0f);
  m_EyeTranslation = OVR::Matrix4f::Identity();
  m_ModelView = OVR::Matrix4f::Identity();
  m_Projection = OVR::Matrix4f::Identity();
  m_FinalUp = OVR::Vector3f(0, 1, 0);
  m_FinalForward = OVR::Vector3f(0, 0, -1);
  //m_HeadView = OVR::Matrix4f::Identity();
  m_Position = OVR::Vector3f(0, 0, 0);

  return true;
}

#if _WIN32
void OculusVR::SetHWND(HWND hwnd) {
  m_HWND = hwnd;
}
#endif

void OculusVR::BeginFrame() {
#if SDK_RENDER
  m_FrameTiming = ovrHmd_BeginFrame(m_HMD, 0);
#else
  m_FrameTiming = ovrHmd_BeginFrameTiming(m_HMD, 0);
#endif
  m_MovePose = ovrHmd_GetSensorState(m_HMD, m_FrameTiming.ScanoutMidpointSeconds).Predicted.Pose;
  
  m_Orientation = m_MovePose.Orientation;
  m_Position = m_MovePose.Position;
  m_Position.x *= 1000.0;
  m_Position.y *= -1000.0;
  m_Position.z *= 1000.0;

  for (int i=0; i<2; i++) {
    m_Eyes[i] = m_HMDDesc.EyeRenderOrder[i];
  }
}

/*
1) set render target
2) set viewport
3) clear
4) setup eye
5) setup matrices
6) render
7) end
*/

void OculusVR::BeginEye(int idx) {
  const ovrEyeType& eye = m_Eyes[idx];

#if SDK_RENDER
  m_EyePose[eye] = ovrHmd_BeginEyeRender(m_HMD, eye);
#else
  m_EyePose[eye] = ovrHmd_GetEyePose(m_HMD, eye);
#endif

  const OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(m_EyeYaw);
  const OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(m_EyePose[eye].Orientation);
  m_FinalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
  m_FinalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
  OVR::Vector3f eyePosition = m_EyePose[eye].Position;
  eyePosition *= 1000.0f;
  const OVR::Vector3f shiftedEyePos = m_EyePos + rollPitchYaw.Transform(eyePosition);
  const OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + m_FinalForward, m_FinalUp);
  //std::cout << "eye pos: " << toCinder(m_EyePose[eye].Position) << std::endl;

  ovrEigenTypes::Vector3f viewAdjust = m_EyeRenderDesc[eye].ViewAdjust;
  viewAdjust.x *= 1000;
  viewAdjust.y *= 1000;
  viewAdjust.z *= 1000;
  m_EyeTranslation = OVR::Matrix4f::Translation(viewAdjust);
  m_ModelView = m_EyeTranslation * view;
  m_Projection = ovrMatrix4f_Projection(m_EyeRenderDesc[eye].Fov, 1.0f, 100000.0f, true);
}

void OculusVR::EndEye(int idx) {
#if SDK_RENDER
  const ovrEyeType& eye = m_Eyes[idx];
  ovrHmd_EndEyeRender(m_HMD, eye, m_EyePose[eye], &m_EyeTexture[eye].Texture);
#endif
}

void OculusVR::EndFrame() {
#if SDK_RENDER
  //ovrHmd
  ovrHmd_EndFrame(m_HMD);
#else
  //DistortionMeshRender(DistortionCaps, HMD, frameTiming.TimewarpPointSeconds, eyeRenderPose, pRender, pRendertargetTexture);
  //pRender->Present(VSyncEnabled);
  //pRender->WaitUntilGpuIdle();  //for lowest latency
  glFlush();
  glFinish();
  ovrHmd_EndFrameTiming(m_HMD);
#endif
}

void OculusVR::DistortionMeshInit() {
  //Generate distortion mesh for each eye
  for (int eyeNum = 0; eyeNum < 2; eyeNum++)
  {
    // Allocate & generate distortion mesh vertices.
    ovrDistortionMesh meshData;
    ovrHmd_CreateDistortionMesh(m_HMD, m_EyeRenderDesc[eyeNum].Eye, m_EyeRenderDesc[eyeNum].Fov, m_DistortionCaps, &meshData);

    ovrHmd_GetRenderScaleAndOffset(m_EyeRenderDesc[eyeNum].Fov, m_RenderTargetSize, m_EyeRenderViewport[eyeNum], (ovrEigenTypes::Vector2f*)m_UVScaleOffset[eyeNum]);
    m_NumVertices = meshData.VertexCount;
    m_NumIndices = meshData.IndexCount;

    const int positionBytes = m_NumVertices * 2 * sizeof(GLfloat);
    const int colorBytes = m_NumVertices * 4 * sizeof(GLubyte);
    const int coordBytes = m_NumVertices * 2 * sizeof(GLfloat);
    const int indexBytes = m_NumIndices * sizeof(GLushort);

    m_PositionBuffer[eyeNum].create(GL_ARRAY_BUFFER);
    m_PositionBuffer[eyeNum].bind();
    m_PositionBuffer[eyeNum].allocate(0, positionBytes, GL_STATIC_DRAW);
    m_PositionBuffer[eyeNum].release();

    m_ColorBuffer[eyeNum].create(GL_ARRAY_BUFFER);
    m_ColorBuffer[eyeNum].bind();
    m_ColorBuffer[eyeNum].allocate(0, colorBytes, GL_STATIC_DRAW);
    m_ColorBuffer[eyeNum].release();

    m_RBuffer[eyeNum].create(GL_ARRAY_BUFFER);
    m_RBuffer[eyeNum].bind();
    m_RBuffer[eyeNum].allocate(0, coordBytes, GL_STATIC_DRAW);
    m_RBuffer[eyeNum].release();

    m_GBuffer[eyeNum].create(GL_ARRAY_BUFFER);
    m_GBuffer[eyeNum].bind();
    m_GBuffer[eyeNum].allocate(0, coordBytes, GL_STATIC_DRAW);
    m_GBuffer[eyeNum].release();

    m_BBuffer[eyeNum].create(GL_ARRAY_BUFFER);
    m_BBuffer[eyeNum].bind();
    m_BBuffer[eyeNum].allocate(0, coordBytes, GL_STATIC_DRAW);
    m_BBuffer[eyeNum].release();

    m_IndexBuffer[eyeNum].create(GL_ELEMENT_ARRAY_BUFFER);
    m_IndexBuffer[eyeNum].bind();
    m_IndexBuffer[eyeNum].allocate(0, indexBytes, GL_STATIC_DRAW);
    m_IndexBuffer[eyeNum].release();

    // copy position, color, and texture coordinate buffers
    GLfloat* positionPtr = (GLfloat*)m_PositionBuffer[eyeNum].map(GL_WRITE_ONLY);
    GLubyte* colorPtr = (GLubyte*)m_ColorBuffer[eyeNum].map(GL_WRITE_ONLY);
    GLfloat* rPtr = (GLfloat*)m_RBuffer[eyeNum].map(GL_WRITE_ONLY);
    GLfloat* gPtr = (GLfloat*)m_GBuffer[eyeNum].map(GL_WRITE_ONLY);
    GLfloat* bPtr = (GLfloat*)m_BBuffer[eyeNum].map(GL_WRITE_ONLY);
    ovrDistortionVertex* ov = meshData.pVertexData;
    for (unsigned int vertNum = 0; vertNum < m_NumVertices; vertNum++) {
#if 1
      positionPtr[2*vertNum] = ov->Pos.x;
      positionPtr[2*vertNum+1] = ov->Pos.y;
      const GLubyte vignette = static_cast<GLubyte>(ov->VignetteFactor * 255.99f);
      const GLubyte timewarp = static_cast<GLubyte>(ov->TimeWarpFactor * 255.99f);
      colorPtr[4*vertNum] = vignette;
      colorPtr[4*vertNum + 1] = vignette;
      colorPtr[4*vertNum + 2] = vignette;
      colorPtr[4*vertNum + 3] = timewarp;
      rPtr[2*vertNum] = ov->TexR.x;
      rPtr[2*vertNum+1] = ov->TexR.y;
      gPtr[2*vertNum] = ov->TexG.x;
      gPtr[2*vertNum+1] = ov->TexG.y;
      bPtr[2*vertNum] = ov->TexB.x;
      bPtr[2*vertNum+1] = ov->TexB.y;
#else
      *(positionPtr++) = ov->Pos.x;
      *(positionPtr++) = ov->Pos.y;
      const GLubyte vignette = static_cast<GLubyte>(ov->VignetteFactor * 255.99f);
      const GLubyte timewarp = static_cast<GLubyte>(ov->TimeWarpFactor * 255.99f);
      *(colorPtr++) = vignette;
      *(colorPtr++) = vignette;
      *(colorPtr++) = vignette;
      *(colorPtr++) = timewarp;
      *(rPtr++) = ov->TexR.x;
      *(rPtr++) = ov->TexR.y;
      *(gPtr++) = ov->TexG.x;
      *(gPtr++) = ov->TexG.y;
      *(bPtr++) = ov->TexB.x;
      *(bPtr++) = ov->TexB.y;
#endif
      ov++;
    }
    m_PositionBuffer[eyeNum].unmap();
    m_ColorBuffer[eyeNum].unmap();
    m_RBuffer[eyeNum].unmap();
    m_GBuffer[eyeNum].unmap();
    m_BBuffer[eyeNum].unmap();
    
    // copy index buffer
    GLushort* indexPtr = (GLushort*)m_IndexBuffer[eyeNum].map(GL_WRITE_ONLY);
    memcpy(indexPtr, meshData.pIndexData, m_NumIndices*sizeof(GLushort));
    m_IndexBuffer[eyeNum].unmap();
    ovrHmd_DestroyDistortionMesh(&meshData);
  }
}

void OculusVR::DistortionMeshRender() {
  ovr_WaitTillTime(m_FrameTiming.TimewarpPointSeconds);
  m_HMDFbo.bindTexture();

  ci::gl::GlslProg& shader = Globals::distortionShader;
  shader.bind();
  m_PositionAttribIdx = shader.getAttribLocation("Position");
  m_ColorAttribIdx = shader.getAttribLocation("Color");
  m_RAttribIdx = shader.getAttribLocation("TexCoord0");
  m_GAttribIdx = shader.getAttribLocation("TexCoord1");
  m_BAttribIdx = shader.getAttribLocation("TexCoord2");
  shader.uniform("Texture0", 0);

  for (int eyeNum = 0; eyeNum < 2; eyeNum++) {
    ovrMatrix4f timeWarpMatrices[2];
    ovrHmd_GetEyeTimewarpMatrices(m_HMD, (ovrEyeType)eyeNum, m_EyePose[eyeNum], timeWarpMatrices);

    ci::Matrix44f start = ci::Matrix44f((float*)timeWarpMatrices[0].M, false).transposed();
    ci::Matrix44f end = ci::Matrix44f((float*)timeWarpMatrices[1].M, false).transposed();

    shader.uniform("EyeToSourceUVScale", ci::Vec2f(m_UVScaleOffset[eyeNum][0].x, m_UVScaleOffset[eyeNum][0].y));
    shader.uniform("EyeToSourceUVOffset", ci::Vec2f(m_UVScaleOffset[eyeNum][1].x, m_UVScaleOffset[eyeNum][1].y));
    shader.uniform("EyeRotationStart", start);
    shader.uniform("EyeRotationEnd", end);

    m_PositionBuffer[eyeNum].bind();
    glEnableVertexAttribArray(m_PositionAttribIdx);
    glVertexAttribPointer(m_PositionAttribIdx, 2, GL_FLOAT, false, 0, 0);

    m_ColorBuffer[eyeNum].bind();
    glEnableVertexAttribArray(m_ColorAttribIdx);
    glVertexAttribPointer(m_ColorAttribIdx, 4, GL_UNSIGNED_BYTE, true, 0, 0);

    m_RBuffer[eyeNum].bind();
    glEnableVertexAttribArray(m_RAttribIdx);
    glVertexAttribPointer(m_RAttribIdx, 2, GL_FLOAT, false, 0, 0);

    m_GBuffer[eyeNum].bind();
    glEnableVertexAttribArray(m_GAttribIdx);
    glVertexAttribPointer(m_GAttribIdx, 2, GL_FLOAT, false, 0, 0);

    m_BBuffer[eyeNum].bind();
    glEnableVertexAttribArray(m_BAttribIdx);
    glVertexAttribPointer(m_BAttribIdx, 2, GL_FLOAT, false, 0, 0);

    m_IndexBuffer[eyeNum].bind();
    glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0);
    m_IndexBuffer[eyeNum].release();
    glDisableVertexAttribArray(m_PositionAttribIdx);
    glDisableVertexAttribArray(m_ColorAttribIdx);
    glDisableVertexAttribArray(m_RAttribIdx);
    glDisableVertexAttribArray(m_GAttribIdx);
    glDisableVertexAttribArray(m_BAttribIdx);

    m_PositionBuffer[eyeNum].release();
    m_ColorBuffer[eyeNum].release();
    m_RBuffer[eyeNum].release();
    m_GBuffer[eyeNum].release();
    m_BBuffer[eyeNum].release();
  }

  m_HMDFbo.unbindTexture();
  shader.unbind();
}
