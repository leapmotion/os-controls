#pragma once

#include "EigenTypes.h"
#include "GLShader.h"
#include "Skeleton.h"
#include "SkinnedVboMesh.h"
#include "GLTexture2Image.h"
#include "PrimitiveBase.h"

#include <Leap.h>

#include <memory>

class RiggedHand : public PrimitiveBase {
public:

  RiggedHand();
  virtual ~RiggedHand();

  enum Gender { MALE, FEMALE, NUM_GENDERS };
  enum SkinTone { DARK, MEDIUM, LIGHT, NUM_SKIN_TONES };

  void SetStyle(Gender gender, SkinTone tone);
  void Update(const Leap::Hand& hand);

  virtual void MakeAdditionalModelViewTransformations(ModelView &model_view) const override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  enum TextureMap { DIFFUSE, NORMAL, SPECULAR, NUM_TEXTURE_MAPS };

  void updateStyle();
  void updateLeapData();
  void updateFinger(int fingerIdx);
  void updateMeshMirroring(bool left);
  void drawLeapHand(const Leap::Hand& hand) const;
  model::NodeRef getArmNode() const;
  model::NodeRef getWristNode() const;
  model::NodeRef getJointNode(int fingerIdx, int boneIdx) const;

  static GLShaderRef getHandsShader();
  static model::SkinnedVboMeshRef getMeshForGender(Gender gender);
  static GLTexture2ImageRef getTexture(Gender gender, SkinTone tone, TextureMap texture);
  static Eigen::Quaterniond lookRotation(const Eigen::Vector3d& lookAt, const Eigen::Vector3d& upDirection, bool leftHanded = false);
  static Eigen::Quaterniond toQuat(const Eigen::Matrix3d& basis, bool leftHanded = false);
  static Eigen::Matrix3d toEigen(const Leap::Matrix& mat);
  static Eigen::Quaterniond computeArmReorientation(bool left);
  static Eigen::Quaterniond computeWristReorientation(bool left);
  static Eigen::Quaterniond computeFingerReorientation(bool left);

  // data transferred from Leap to rig
  Leap::Hand mHand;
  bool mPrevIsLeft;
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
  Eigen::Vector3f	mLightPos;

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
  GLShaderRef mHandsShader;
};

typedef std::shared_ptr<RiggedHand> RiggedHandRef;
