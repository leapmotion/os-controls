#ifndef __OculusVR_h__
#define __OculusVR_h__

typedef struct ovrGLConfigData_s
{
  // General device settings.
  ovrRenderAPIConfigHeader Header;

#if _WIN32
  HWND   Window;
#endif
} ovrGLConfigData;

union ovrGLConfig
{
  ovrRenderAPIConfig Config;
  ovrGLConfigData OGL;
};

// Used to pass GL eye texture data to ovrHmd_EndFrame.
typedef struct ovrGLTextureData_s
{
  // General device settings.
  ovrTextureHeader          Header;
  GLuint           TexId;
} ovrGLTextureData;

typedef union ovrGLTexture_s
{
  ovrTexture          Texture;
  ovrGLTextureData	OGL;
} ovrGLTexture;


class OculusVR {
public:
  bool Init();
#if _WIN32
  void SetHWND(HWND hwnd);
#endif

  void BeginFrame();
  void BeginEye(int idx);

  void EndEye(int idx);
  void EndFrame();

  void DistortionMeshInit();
  void DistortionMeshRender();

  //Format for mesh and shaders
  struct DistortionVertex
  {
    OVR::Vector2f Pos;
    OVR::Vector2f TexR;
    OVR::Vector2f TexG;
    OVR::Vector2f TexB;
    OVR::Color Col;
  };

//private:
  ovrHmd m_HMD;
  ovrHmdDesc m_HMDDesc;
  ovrSizei m_LeftTexSize;
  ovrSizei m_RightTexSize;
  ovrEyeRenderDesc m_EyeRenderDesc[2];
  ovrSizei m_RenderTargetSize;
  ovrFrameTiming m_FrameTiming;
  ovrPosef m_MovePose;
  ovrEyeType m_Eyes[2];
  ovrPosef m_EyePose[2];
  ovrRecti m_EyeRenderViewport[2];
  unsigned int m_DistortionCaps;
  OVR::Vector2f m_UVScaleOffset[2][2];
  GLBuffer m_PositionBuffer[2];
  GLBuffer m_ColorBuffer[2];
  GLBuffer m_RBuffer[2];
  GLBuffer m_GBuffer[2];
  GLBuffer m_BBuffer[2];
  GLBuffer m_IndexBuffer[2];
  unsigned int m_NumVertices;
  unsigned int m_NumIndices;

  int m_PositionAttribIdx;
  int m_ColorAttribIdx;
  int m_RAttribIdx;
  int m_GAttribIdx;
  int m_BAttribIdx;

  OVR::Vector3f m_EyePos;
  OVR::Matrix4f m_EyeTranslation;
  OVR::Matrix4f m_ModelView;
  OVR::Matrix4f m_Projection;
  OVR::Vector3f m_FinalUp;
  OVR::Vector3f m_FinalForward;
  OVR::Vector3f m_Position;
  OVR::Quatf m_Orientation;

  //OVR::Matrix4f m_HeadView;
  float m_EyeYaw;
#if _WIN32
  HWND m_HWND;
#endif
  ovrGLTexture m_EyeTexture[2];
  ci::gl::Fbo m_HMDFbo;
  ovrDistortionMesh meshData[2];
};

//! Converts OVR Matrices to Cinder Matrices
inline ci::Matrix44f toCinder(const OVR::Matrix4f& ovrMat)
{
  return ci::Matrix44f(ovrMat.M[0][0], ovrMat.M[0][1], ovrMat.M[0][2], ovrMat.M[0][3],
    ovrMat.M[1][0], ovrMat.M[1][1], ovrMat.M[1][2], ovrMat.M[1][3],
    ovrMat.M[2][0], ovrMat.M[2][1], ovrMat.M[2][2], ovrMat.M[2][3],
    ovrMat.M[3][0], ovrMat.M[3][1], ovrMat.M[3][2], ovrMat.M[3][3]).transposed();
}
//! Converts OVR Quaternions to Cinder Quaternions
inline ci::Quatf toCinder(const OVR::Quatf& ovrQuat){
  return ci::Quatf(ovrQuat.w, ovrQuat.x, ovrQuat.y, ovrQuat.z);
}
//! Converts OVR Vector3f to Cinder Vec3f
inline ci::Vec3f toCinder(const OVR::Vector3f& ovrVec){
  return ci::Vec3f(ovrVec.x, ovrVec.y, ovrVec.z);
}
inline ci::Area toCinder(const ovrRecti& rect) {
  return ci::Area(rect.Pos.x, rect.Pos.y, rect.Pos.x + rect.Size.w, rect.Pos.y + rect.Size.h);
}

inline Vector3 toVector3(const OVR::Vector3f& ovrVec) {
  return Vector3(ovrVec.x, ovrVec.y, ovrVec.z);
}

inline Matrix4x4 toMatrix4x4(const OVR::Matrix4f& ovrMat) {
  Matrix4x4 result(Matrix4x4::Identity());
  result << ovrMat.M[0][0], ovrMat.M[0][1], ovrMat.M[0][2], ovrMat.M[0][3],
    ovrMat.M[1][0], ovrMat.M[1][1], ovrMat.M[1][2], ovrMat.M[1][3],
    ovrMat.M[2][0], ovrMat.M[2][1], ovrMat.M[2][2], ovrMat.M[2][3],
    ovrMat.M[3][0], ovrMat.M[3][1], ovrMat.M[3][2], ovrMat.M[3][3];
  return result.transpose();
}

inline Matrix4x4 toMatrix4x4(const ci::Matrix44f& matrix) {
  Matrix4x4 result(Matrix4x4::Identity());
  result << matrix.at(0, 0), matrix.at(0, 1), matrix.at(0, 2), matrix.at(0, 3),
    matrix.at(1, 0), matrix.at(1, 1), matrix.at(1, 2), matrix.at(1, 3),
    matrix.at(2, 0), matrix.at(2, 1), matrix.at(2, 2), matrix.at(2, 3),
    matrix.at(3, 0), matrix.at(3, 1), matrix.at(3, 2), matrix.at(3, 3);
  return result;
}

#endif
