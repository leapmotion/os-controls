#pragma once

#include "EigenTypes.h"
#include "GLShader.h"
#include "Skeleton.h"
#include "SkinnedVboMesh.h"
#include "GLTexture2Image.h"
#include "PrimitiveBase.h"

#include <memory>

class RiggedHand : public PrimitiveBase {
public:

  RiggedHand();
  virtual ~RiggedHand();

  enum Gender { MALE, FEMALE, NUM_GENDERS };
  enum SkinTone { DARK, MEDIUM, LIGHT, NUM_SKIN_TONES };

  void SetStyle(Gender gender, SkinTone tone);

  void SetConfidence(float confidence);
  void SetTimeVisible(float timeVisible);
  void SetIsLeft(bool isLeft);
  void SetArmBasis(const Eigen::Matrix3f& armBasis);
  void SetHandBasis(const Eigen::Matrix3f& handBasis);
  void SetPalmWidth(float palmWidth);
  void SetWristPosition(const Eigen::Vector3f& wristPosition);
  void SetBoneBasis(int fingerIdx, int boneIdx, const Eigen::Matrix3f& basis);
  void SetBoneLength(int fingerIdx, int boneIdx, float length);

  // after setting data call Update to transfer data to rig/skin
  void UpdateRigAndSkin();

  virtual void MakeAdditionalModelViewTransformations(ModelView &model_view) const override;

  void SetHandsShader(const GLShaderRef& shader) { mHandsShader = shader; };
  GLShaderRef HandsShader() { return mHandsShader; };

  static GLShaderRef getDefaultHandsShader();

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  enum TextureMap { DIFFUSE, NORMAL, SPECULAR, NUM_TEXTURE_MAPS };

  void updateStyle();
  void updateIntermediateData();
  void updateFinger(int fingerIdx);
  void updateMeshMirroring(bool left);
  void setOffsetsForGender();
  model::NodeRef getArmNode() const;
  model::NodeRef getWristNode() const;
  model::NodeRef getJointNode(int fingerIdx, int boneIdx) const;

  static model::SkinnedVboMeshRef getMeshForGender(Gender gender);
  static GLTexture2ImageRef getTexture(Gender gender, SkinTone tone, TextureMap texture);
  static Eigen::Quaterniond lookRotation(const Eigen::Vector3d& lookAt, const Eigen::Vector3d& upDirection, bool leftHanded = false);
  static Eigen::Quaterniond toQuat(const Eigen::Matrix3d& basis, bool leftHanded = false);
  static Eigen::Quaterniond computeArmReorientation(bool left);
  static Eigen::Quaterniond computeWristReorientation(bool left);
  static Eigen::Quaterniond computeFingerReorientation(bool left);

  // data transferred from Leap to rig
  float mConfidence;
  float mTimeVisible;
  bool mIsLeft;
  Eigen::Matrix3d mArmBasis;
  Eigen::Matrix3d mHandBasis;
  float mPalmWidth;
  Eigen::Vector3f mLeapWristPosition;
  Eigen::Matrix3d mBoneBases[5][4];
  float mBoneLengths[5][4];

  // intermediate data computed from Leap data
  bool mPrevIsLeft;
  Eigen::Vector3f mWristPosition;
  Eigen::Vector3f mElbowPos;
  Eigen::Quaterniond mArmRotation;
  Eigen::Quaterniond mWristRotation;
  Eigen::Quaterniond mHandRotation;
  float mArmScale;

  // drawing controls
  bool mDrawSkeleton;
  bool mDrawLabels;
  bool mDrawMesh;
  bool mEnableWireframe;
  bool mDrawLeap;
  bool mDrawConfidence;

  // material properties
  bool mUseNormalMap;
  bool mUseSpecularMap;
  bool mUseRim;
  float mOpacity;
  float mRimStart;
  float mSpecular;
  float mShininess;
  float mAmbient;
  float mDiffuse;
  float mInnerTransparency;

  // rigged hand rendering
  Gender mGender;
  SkinTone mSkinTone;
  Gender mPrevGender;
  SkinTone mPrevSkinTone;
  model::SkinnedVboMeshRef mSkinnedVboHands;

  // textures for diffuse skin, normal map, and specular map
  GLTexture2ImageRef mSkinTex;
  GLTexture2ImageRef mNormalTex;
  GLTexture2ImageRef mSpecularTex;

  // configurable offsets to align rig to Leap hand
  float mScaleMultiplier;
  Eigen::Vector3f mTranslationOffset;
  Eigen::Quaternionf mRotationOffset;

  // rotation and translation values needed for transforming rig
  float mForearmLength;
  Eigen::Quaterniond mArmReorientation;
  Eigen::Quaterniond mWristReorientation;
  Eigen::Quaterniond mFingerReorientation;

  // hand shader
  mutable GLShaderRef mHandsShader;

  // convert units on loading
  static const float UNIT_CONVERSION_SCALE_FACTOR; 
};

typedef std::shared_ptr<RiggedHand> RiggedHandRef;
