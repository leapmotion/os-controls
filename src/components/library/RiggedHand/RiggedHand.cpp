#include "RiggedHand.h"

#include "Leap/GL/GLShader.h"
#include "ModelIo.h"
#include "ModelSourceAssimp.h"

#include <cmath>
#include <assert.h>

const float RiggedHand::UNIT_CONVERSION_SCALE_FACTOR = 10.0f; // FBX model is in cm, our units are mm

RiggedHand::RiggedHand() {
  mConfidence = 0.0f;
  mTimeVisible = 0.0f;
  mIsLeft = false;
  mArmBasis.setIdentity();
  mHandBasis.setIdentity();
  mPalmWidth = 1.0f;
  mLeapWristPosition.setZero();
  for (int i=0; i<5; i++) {
    for (int j=0; j<4; j++) {
      mBoneBases[i][j].setIdentity();
      mBoneLengths[i][j] = 1.0f;
    }
  }

  mPrevIsLeft = false;
  mWristPosition.setZero();
  mElbowPos.setZero();
  mArmRotation.setIdentity();
  mWristRotation.setIdentity();
  mHandRotation.setIdentity();
  mArmScale = 1.0f;

  mDrawSkeleton = false;
  mDrawLabels = false;
  mDrawMesh = true;
  mEnableWireframe = false;
  mDrawLeap = false;
  mDrawConfidence = false;

  mUseNormalMap = true;
  mUseSpecularMap = true;
  mUseRim = false;
  mOpacity = 1.0f;
  mRimStart = 0.25f;
  mSpecular = 0.4f;
  mShininess = 5.0f;
  mAmbient = 0.00f;
  mDiffuse = 0.5f;
  mInnerTransparency = 0.0f;

  mGender = FEMALE;
  mSkinTone = MEDIUM;
  mPrevGender = NUM_GENDERS;
  mPrevSkinTone = NUM_SKIN_TONES;

  setOffsetsForGender();

  mForearmLength = 1.0f;
  mArmReorientation = computeArmReorientation(mPrevIsLeft);
  mWristReorientation = computeWristReorientation(mPrevIsLeft);
  mFingerReorientation = computeFingerReorientation(mPrevIsLeft);
}

RiggedHand::~RiggedHand() {

}

void RiggedHand::SetStyle(Gender gender, SkinTone tone) {
  mGender = gender;
  mSkinTone = tone;
}

void RiggedHand::SetConfidence(float confidence) {
  mConfidence = confidence;
}

void RiggedHand::SetTimeVisible(float timeVisible) {
  mTimeVisible = timeVisible;
}

void RiggedHand::SetIsLeft(bool isLeft) {
  mIsLeft = isLeft;
}

void RiggedHand::SetArmBasis(const Eigen::Matrix3f& armBasis) {
  mArmBasis = armBasis.cast<double>();
}

void RiggedHand::SetHandBasis(const Eigen::Matrix3f& handBasis) {
  mHandBasis = handBasis.cast<double>();
}

void RiggedHand::SetPalmWidth(float palmWidth) {
  mPalmWidth = palmWidth;
}

void RiggedHand::SetWristPosition(const Eigen::Vector3f& wristPosition) {
  mLeapWristPosition = wristPosition;
}

void RiggedHand::SetBoneBasis(int fingerIdx, int boneIdx, const Eigen::Matrix3f& basis) {
  mBoneBases[fingerIdx][boneIdx] = basis.cast<double>();
}

void RiggedHand::SetBoneLength(int fingerIdx, int boneIdx, float length) {
  mBoneLengths[fingerIdx][boneIdx] = length;
}

void RiggedHand::UpdateRigAndSkin() {
  updateStyle();
  updateIntermediateData();

  model::NodeRef armNode = getArmNode();
  model::NodeRef wristNode = getWristNode();

  armNode->setRelativeRotation(mArmRotation.cast<float>());
  armNode->setRelativePosition(mElbowPos);
  armNode->setRelativeScale(Eigen::Vector3f::Constant(mArmScale));
  wristNode->setRelativeRotation(mWristRotation.cast<float>());

  const float confidenceMult = mDrawConfidence ? std::min(1.0f, mConfidence) : 1.0f;
  const float timeVisibleMult = std::min(1.0f, 0.5f * mTimeVisible);
  mOpacity = confidenceMult * timeVisibleMult;

  mSkinnedVboHands->update();
}

void RiggedHand::MakeAdditionalModelViewTransformations(ModelView& model_view) const {

}

void RiggedHand::DrawContents(RenderState& renderState) const {
  if (!mHandsShader) {
    mHandsShader = getDefaultHandsShader();
  }

  if (!mShaderMatrices) {
    mShaderMatrices =
      std::make_shared<ShaderMatrices>(
      *mHandsShader,
      "projection_times_model_view_matrix",
      "model_view_matrix",
      "normal_matrix");
  }

  std::vector<model::MeshVboSectionRef>& sections = mSkinnedVboHands->getSections();
  if (mEnableWireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  const int startIdx = mGender == MALE ? 1 : 2;
  const int numSections = mGender == MALE ? 1 : 2;
  for (int sectionIdx = startIdx; sectionIdx < (startIdx + numSections); sectionIdx++) {
    model::MeshVboSectionRef section = sections[sectionIdx];
    if (section->hasDefaultTransformation()) {
      renderState.GetModelView().Push();
      const EigenTypes::Matrix4x4 defaultTrans = section->getDefaultTranformation().cast<double>();
      renderState.GetModelView().Multiply(defaultTrans);
    }

    if (mDrawMesh) {
      mSkinTex->Bind(0);
      mNormalTex->Bind(1);
      mSpecularTex->Bind(2);
      Rgba<float> rimColor = mUseRim ? Rgba<float>(0.075f, 0.1f, 0.125f, 1.0f) : Rgba<float>(0.0f, 0.0f, 0.0f, 1.0f);
      Rgba<float> specularColor = Rgba<float>(mSpecular, mSpecular, mSpecular, 1.0f);
      Rgba<float> ambientColor = Rgba<float>(mAmbient, mAmbient, mAmbient, 1.0f);
      Rgba<float> diffuseColor = Rgba<float>(mDiffuse, mDiffuse, mDiffuse, 1.0f);

      mHandsShader->Bind();

      //const Eigen::Matrix4f modelView =  renderState.GetModelView().Matrix().cast<float>().eval();
      //mHandsShader->SetUniformMatrixf<4, 4, Eigen::Matrix4f>("modelView", modelView, MatrixStorageConvention::COLUMN_MAJOR);
      //const Eigen::Matrix4f projection = renderState.GetProjection().Matrix().cast<float>().eval();
      //mHandsShader->SetUniformMatrixf<4, 4, Eigen::Matrix4f>("projection", projection, MatrixStorageConvention::COLUMN_MAJOR);
      //const Eigen::Matrix4f normalMatrix = renderState.GetModelView().Matrix().inverse().transpose().cast<float>().eval();
      //mHandsShader->SetUniformMatrixf<4, 4, Eigen::Matrix4f>("normalMatrix", normalMatrix, MatrixStorageConvention::COLUMN_MAJOR);

      // GLShaderMatrices::UploadUniforms(*mHandsShader, renderState.GetModelView().Matrix(), renderState.GetProjection().Matrix(), BindFlags::NONE);
      mShaderMatrices->SetMatrices(renderState.GetModelView().Matrix(), renderState.GetProjection().Matrix());
      mShaderMatrices->UploadUniforms();


      mHandsShader->UploadUniform<GL_BOOL>("isAnimated", section->isAnimated());
      mHandsShader->UploadUniform<GL_BOOL>("use_texture", true);
      mHandsShader->UploadUniform<GL_SAMPLER_2D>("texture", 0);
      mHandsShader->UploadUniform<GL_BOOL>("useNormalMap", mUseNormalMap);
      mHandsShader->UploadUniform<GL_SAMPLER_2D>("normalMap", 1);
      mHandsShader->UploadUniform<GL_BOOL>("useSpecularMap", mUseSpecularMap);
      mHandsShader->UploadUniform<GL_SAMPLER_2D>("specularMap", 2);

      mHandsShader->UploadUniform<GL_FLOAT_VEC4>("diffuse_light_color", diffuseColor);
      mHandsShader->UploadUniform<GL_FLOAT_VEC4>("specular", specularColor);
      mHandsShader->UploadUniform<GL_FLOAT_VEC4>("ambient_light_color", ambientColor);
      mHandsShader->UploadUniform<GL_FLOAT>("ambient_lighting_proportion", 0.0f);
      mHandsShader->UploadUniform<GL_FLOAT>("shininess", mShininess);

      if (section->hasSkeleton()) {
        // const int boneMatricesAddr = mHandsShader->LocationOfUniform("boneMatrices[0]");
        // const int invTransposeMatricesAddr = mHandsShader->LocationOfUniform("invTransposeMatrices[0]");
        const int boneMatricesAddr = mHandsShader->LocationOfUniform("boneMatrices");
        const int invTransposeMatricesAddr = mHandsShader->LocationOfUniform("invTransposeMatrices");

        // TODO: replace with GLShader::UploadUniform
        glUniformMatrix4fv(boneMatricesAddr, model::SkinnedVboMesh::MAXBONES, false, section->mBoneMatricesPtr->data()->data());
        glUniformMatrix4fv(invTransposeMatricesAddr, model::SkinnedVboMesh::MAXBONES, false, section->mInvTransposeMatricesPtr->data()->data());
      }

      const int positionAddr = mHandsShader->LocationOfAttribute("position");
      const int normalAddr = mHandsShader->LocationOfAttribute("normal");
      const int texcoordAddr = mHandsShader->LocationOfAttribute("tex_coord");
      const int boneWeightsAddr = mHandsShader->LocationOfAttribute("boneWeights");
      const int boneIndicesAddr = mHandsShader->LocationOfAttribute("boneIndices");

      auto locations = std::make_tuple(positionAddr, normalAddr, texcoordAddr, boneWeightsAddr, boneIndicesAddr);
      section->getVboMesh().Enable(locations);

      const int numIndices = static_cast<int>(section->getIndices().Size())/sizeof(uint32_t);
      section->getIndices().Bind();
      GL_THROW_UPON_ERROR(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
      section->getIndices().Unbind();

      section->getVboMesh().Disable(locations);
      mHandsShader->Unbind();
    }

    /*if (mDrawSkeleton) {
    SkinningRenderer::draw(mSkinnedVboHands->getSkeleton(), true, "Bip01 R Forearm");
    }*/

    if (section->hasDefaultTransformation()) {
      renderState.GetModelView().Pop();
    }

    mSpecularTex->Unbind();
    mNormalTex->Unbind();
    mSkinTex->Unbind();
  }
  if (mEnableWireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void RiggedHand::updateStyle() {
  // load new mesh if necessary
  if (mGender != mPrevGender) {
    mSkinnedVboHands = getMeshForGender(mGender);
    setOffsetsForGender();

    mSkinnedVboHands->update();
    mForearmLength = (getArmNode()->getAbsolutePosition() - getWristNode()->getAbsolutePosition()).norm();
  }

  // load new skin textures if necessary
  if (mGender != mPrevGender || mSkinTone != mPrevSkinTone) {
    mSkinTex = getTexture(mGender, mSkinTone, DIFFUSE);
    mNormalTex = getTexture(mGender, mSkinTone, NORMAL);
    mSpecularTex = getTexture(mGender, mSkinTone, SPECULAR);
  }

  mPrevGender = mGender;
  mPrevSkinTone = mSkinTone;
}

void RiggedHand::updateIntermediateData() {
  if (mIsLeft != mPrevIsLeft) {
    mArmReorientation = computeArmReorientation(mIsLeft);
    mWristReorientation = computeWristReorientation(mIsLeft);
    mFingerReorientation = computeFingerReorientation(mIsLeft);
    mPrevIsLeft = mIsLeft;
  }

  const Eigen::Vector3f handDirection = mHandBasis.col(2).cast<float>();

  mArmScale = mScaleMultiplier * mPalmWidth / 100;

  const Eigen::Vector3f wristPos = mLeapWristPosition - mArmScale*(mHandBasis.cast<float>()*mTranslationOffset);

  const Eigen::Vector3f forearmDir = -mArmBasis.col(2).cast<float>();
  const Eigen::Vector3f forearmDirNormalized = forearmDir.normalized();
  mElbowPos = wristPos - mArmScale * mForearmLength * forearmDirNormalized;

  const Eigen::Quaterniond armQuat = toQuat(mArmBasis, mIsLeft);
  Eigen::Quaterniond handednessQuat = armQuat;
  if (mIsLeft) {
    handednessQuat.y() *= -1.0;
    handednessQuat.z() *= -1.0;
  }
  mArmRotation = handednessQuat * mArmReorientation;

  mHandRotation = toQuat(mHandBasis, mIsLeft);

  Eigen::Quaterniond rotationOffset = mRotationOffset.cast<double>();
  if (mIsLeft) {
    rotationOffset.x() *= -1.0;
    rotationOffset.y() *= -1.0;
  }

  mWristRotation = mWristReorientation * rotationOffset * armQuat.inverse() * mHandRotation;
  std::swap(mWristRotation.x(), mWristRotation.z());
  mWristRotation.x() *= -1;
  if (!mIsLeft) {
    mWristRotation.y() *= -1;
    mWristRotation.z() *= -1;
  }

  for (int i=0; i<5; i++) {
    updateFinger(i);
  }

  updateMeshMirroring(mIsLeft);
}

void RiggedHand::updateFinger(int fingerIdx) {
  Eigen::Quaterniond prevQuat = mHandRotation * mFingerReorientation;
  for (int boneIdx=0; boneIdx<3; boneIdx++) {
    const Eigen::Matrix3d& boneBasis = mBoneBases[fingerIdx][boneIdx+1];

    const Eigen::Quaterniond boneQuat = toQuat(boneBasis, mIsLeft) * mFingerReorientation;
    model::NodeRef boneNode = getJointNode(fingerIdx, boneIdx);
    boneNode->setRelativeRotation((prevQuat.inverse() * boneQuat).cast<float>());

    const int minBoneToAdjust = fingerIdx == 0 ? 2 : 1;
    if (boneIdx >= minBoneToAdjust) {
      //Leap::Bone prevBone = finger.bone(static_cast<Leap::Bone::Type>(boneIdx));
      Eigen::Vector3f relativePos = boneNode->getRelativePosition();
      const float relativePosLength = relativePos.norm();
      relativePos = (UNIT_CONVERSION_SCALE_FACTOR / 10.0f) * mBoneLengths[fingerIdx][boneIdx] * (relativePos / relativePosLength) / mArmScale;
      boneNode->setRelativePosition(relativePos);
    }

    prevQuat = boneQuat;
  }
}

void RiggedHand::updateMeshMirroring(bool left) {
  std::vector<model::MeshVboSectionRef>& sections = mSkinnedVboHands->getSections();
  assert(sections.size() > 1);
  Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();
  if (left) {
    Eigen::Matrix4f translation = Eigen::Matrix4f::Identity();
    translation.col(3) << mElbowPos.x(), mElbowPos.y(), mElbowPos.z(), 1.0f;
    transform = (transform * translation).eval();
    transform = (transform * Eigen::Vector4f(-1, 1, 1, 1).asDiagonal()).eval();
    translation.col(3) << -mElbowPos.x(), -mElbowPos.y(), -mElbowPos.z(), 1.0f;
    transform = (transform * translation).eval();
  }
  if (mGender == MALE) {
    model::SkinnedVboMesh::MeshVboSectionRef& modelSection = sections[1];
    modelSection->setDefaultTransformation(transform);
  } else if (mGender == FEMALE) {
    model::SkinnedVboMesh::MeshVboSectionRef& modelSection = sections[2];
    model::SkinnedVboMesh::MeshVboSectionRef& nailsSection = sections[3];
    modelSection->setDefaultTransformation(transform);
    nailsSection->setDefaultTransformation(transform);
  }
}

void RiggedHand::setOffsetsForGender() {
  if (mGender == FEMALE) {
    mScaleMultiplier = 1.26f;
    mTranslationOffset = Eigen::Vector3f(-3.0f, -2.0f, -14.0f);
    mRotationOffset = Eigen::Quaternionf(1.0f, 0.01f, 0.02f, 0.01f);
  } else if (mGender == MALE) {
    mScaleMultiplier = 1.16f;
    mTranslationOffset = Eigen::Vector3f(5.0f, -2.0f, -11.0f);
    mRotationOffset = Eigen::Quaternionf(1.0f, 0.025f, 0.025f, 0.045f);
  }
}

model::NodeRef RiggedHand::getArmNode() const {
  const int childIdx = mGender == MALE ? 2 : 3;
  return mSkinnedVboHands->getSkeleton()->getRootNode()->getChildren()[childIdx];
}

model::NodeRef RiggedHand::getWristNode() const {
  return mSkinnedVboHands->getSkeleton()->getNode("Bip01 R Hand");
}

model::NodeRef RiggedHand::getJointNode(int fingerIdx, int boneIdx) const {
  static const std::string leftNames[] ={
    "Bip01 R Finger043",
    "Bip01 R Finger044",
    "Bip01 R Finger045",
    "Bip01 R Finger046",
    "Bip01 R Finger047",
    "Bip01 R Finger048",
    "Bip01 R Finger049",
    "Bip01 R Finger050",
    "Bip01 R Finger051",
    "Bip01 R Finger052",
    "Bip01 R Finger053",
    "Bip01 R Finger054",
    "Bip01 R Finger055",
    "Bip01 R Finger056",
    "Bip01 R Finger057"
  };
  static const std::string rightNames[] ={
    "Bip01 R Finger0",
    "Bip01 R Finger01",
    "Bip01 R Finger02",
    "Bip01 R Finger1",
    "Bip01 R Finger11",
    "Bip01 R Finger12",
    "Bip01 R Finger2",
    "Bip01 R Finger21",
    "Bip01 R Finger22",
    "Bip01 R Finger3",
    "Bip01 R Finger31",
    "Bip01 R Finger32",
    "Bip01 R Finger4",
    "Bip01 R Finger41",
    "Bip01 R Finger42"
  };

  const int boneNameIdx = fingerIdx*3 + boneIdx;
  const std::string& boneName = rightNames[boneNameIdx];
  return mSkinnedVboHands->getSkeleton()->getBone(boneName);
}

std::shared_ptr<GLShader> RiggedHand::getDefaultHandsShader() {
  static std::shared_ptr<GLShader> handsShader;
  if (handsShader == nullptr) {
    std::shared_ptr<TextFile> lightingFrag(new TextFile("shaders/lighting-frag.glsl"));
    std::shared_ptr<TextFile> lightingVert(new TextFile("shaders/lighting-vert.glsl"));
    handsShader = std::shared_ptr<GLShader>(new GLShader(lightingVert->Contents(), lightingFrag->Contents()));
  }
  return handsShader;
}

model::SkinnedVboMeshRef RiggedHand::getMeshForGender(Gender gender) {
  static model::ModelSourceRef maleMeshSource;
  static model::ModelSourceRef femaleMeshSource;
  if (gender == MALE) {
    if (maleMeshSource == nullptr) {
      maleMeshSource = model::loadModel("models/Male_Rigged_Arm.FBX", "", UNIT_CONVERSION_SCALE_FACTOR);
    }
    return model::SkinnedVboMesh::create(maleMeshSource, nullptr);
  } else if (gender == FEMALE) {
    if (femaleMeshSource == nullptr) {
      femaleMeshSource = model::loadModel("models/Female_Rigged_Arm.FBX", "", UNIT_CONVERSION_SCALE_FACTOR);
    }
    return model::SkinnedVboMesh::create(femaleMeshSource, nullptr);
  } else {
    assert(0);
    return nullptr;
  }
}

GLTexture2ImageRef RiggedHand::getTexture(Gender gender, SkinTone tone, TextureMap texture) {
  static GLTexture2ImageRef textures[NUM_GENDERS * NUM_SKIN_TONES * NUM_TEXTURE_MAPS];
  const int genderIdx = static_cast<int>(gender);
  const int toneIdx = static_cast<int>(tone);
  const int textureIdx = static_cast<int>(texture);
  const int numGenders = static_cast<int>(NUM_GENDERS);
  const int numSkinTones = static_cast<int>(NUM_SKIN_TONES);
  const int numTextureMaps = static_cast<int>(NUM_TEXTURE_MAPS);

  const int itemsPerGender = numSkinTones * numTextureMaps;

  const int idx = genderIdx*itemsPerGender + toneIdx*numTextureMaps + textureIdx;
  if (textures[idx] == nullptr) {
    std::stringstream ss;
    ss << "textures/";
    if (gender == MALE) {
      ss << "Male";
    } else if (gender == FEMALE) {
      ss << "Female";
    } else {
      assert(0);
    }
    ss << "_Arm_";
    if (tone == DARK) {
      ss << "Dark";
    } else if (tone == MEDIUM) {
      ss << "Medium";
    } else if (tone == LIGHT) {
      ss << "Light";
    } else {
      assert(0);
    }
    ss << "Skin_";
    if (texture == DIFFUSE) {
      ss << "color";
    } else if (texture == NORMAL) {
      ss << "normal";
    } else if (texture == SPECULAR) {
      ss << "specular";
    } else {
      assert(0);
    }
    ss << ".png";
    textures[idx] = GLTexture2ImageRef(new GLTexture2Image());
    textures[idx]->LoadPath(ss.str());
  }
  return textures[idx];
}

Eigen::Quaterniond RiggedHand::lookRotation(const Eigen::Vector3d& lookAt, const Eigen::Vector3d& upDirection, bool leftHanded) {
  // re-orthogonalize forward and up for safety
  const Eigen::Vector3d up = upDirection.normalized();
  const Eigen::Vector3d proj = lookAt.dot(up) * up;
  const Eigen::Vector3d forward = (lookAt - proj).normalized();

  // compute perpendicular vector
  const Eigen::Vector3d right = up.cross(forward);

  // compute resulting quaternion from basis
  Eigen::Quaterniond ret;
  ret.w() = std::sqrt(1.0 + right.x() + up.y() + forward.z()) * 0.5;
  const double wRecip = 1.0 / (4.0*ret.w() + 0.00000001);
  ret.x() = (up.z() - forward.y()) * wRecip;
  ret.y() = (forward.x() - right.z()) * wRecip;
  ret.z() = (right.y() - up.x()) * wRecip;

  return ret;
}

Eigen::Quaterniond RiggedHand::toQuat(const Eigen::Matrix3d& basis, bool leftHanded) {
  Eigen::Matrix3d rightHanded = basis;
  if (leftHanded) {
    rightHanded.col(2) *= -1.0;
  }
  static const Eigen::Vector3d LEAP_UP = Eigen::Vector3d::UnitY();
  static const Eigen::Vector3d LEAP_FORWARD = Eigen::Vector3d::UnitZ();
  const Eigen::Vector3d up = rightHanded * LEAP_UP;
  const Eigen::Vector3d forward = rightHanded * LEAP_FORWARD;
  return lookRotation(forward, up, leftHanded);
}

Eigen::Quaterniond RiggedHand::computeArmReorientation(bool left) {
  Eigen::Quaterniond armReorientation(Eigen::AngleAxisd(-M_PI/2.0, Eigen::Vector3d::UnitX()));
  if (left) {
    const Eigen::Quaterniond rot(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY()));
    armReorientation = rot * armReorientation;
  }
  return armReorientation;
}

Eigen::Quaterniond RiggedHand::computeWristReorientation(bool left) {
  Eigen::Quaterniond wristReorientation(Eigen::AngleAxisd(-M_PI/2.0, Eigen::Vector3d::UnitZ()));
  return wristReorientation;
}

Eigen::Quaterniond RiggedHand::computeFingerReorientation(bool left) {
  Eigen::Quaterniond fingerReorientation(Eigen::AngleAxisd(M_PI/2.0, Eigen::Vector3d::UnitY()));
  if (!left) {
    const Eigen::Quaterniond rot(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitZ()));
    fingerReorientation = rot * fingerReorientation;
  }
  return fingerReorientation;
}
