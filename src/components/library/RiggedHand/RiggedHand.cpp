#include "RiggedHand.h"

#include "GLShader.h"
#include "ModelIo.h"
#include "ModelSourceAssimp.h"

#include <cmath>
#include <assert.h>

RiggedHand::RiggedHand() {
  mPrevIsLeft = false;
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
  mUseRim = true;
  mOpacity = 1.0f;
  mRimStart = 0.25f;
  mSpecular = 0.1f;
  mShininess = 6.0f;
  mAmbient = 0.3f;
  mDiffuse = 0.65f;
  mInnerTransparency = 0.0f;
  mLightPos = Eigen::Vector3f(0.0f, 100.0f, 50.0f);

  mGender = FEMALE;
  mSkinTone = MEDIUM;
  mPrevGender = NUM_GENDERS;
  mPrevSkinTone = NUM_SKIN_TONES;

  mScaleMultiplier = 1.27f;
  mTranslationOffset = Eigen::Vector3f(-0.3f, -0.2f, -1.4f);
  mRotationOffset = Eigen::Quaternionf(1.0f, 0.01f, 0.02f, 0.01f);

  mForearmLength = 1.0f;
  mArmReorientation = computeArmReorientation(mPrevIsLeft);
  mWristReorientation = computeWristReorientation(mPrevIsLeft);
  mFingerReorientation = computeFingerReorientation(mPrevIsLeft);

  mHandsShader = getHandsShader();
}

RiggedHand::~RiggedHand() {

}

void RiggedHand::SetStyle(Gender gender, SkinTone tone) {
  mGender = gender;
  mSkinTone = tone;
}

void RiggedHand::Update(const Leap::Hand& hand) {
  mHand = hand;

  updateStyle();
  updateLeapData();

  model::NodeRef armNode = getArmNode();
  model::NodeRef wristNode = getWristNode();

  armNode->setRelativeRotation(mArmRotation.cast<float>());
  armNode->setRelativePosition(mElbowPos);
  armNode->setRelativeScale(Eigen::Vector3f::Constant(mArmScale));
  wristNode->setRelativeRotation(mWristRotation.cast<float>());

  const Leap::FingerList fingers = mHand.fingers();
  for (int i=0; i<fingers.count(); i++) {
    updateFinger(i);
  }

  const float confidenceMult = mDrawConfidence ? std::min(1.0f, mHand.confidence()) : 1.0f;
  const float timeVisibleMult = std::min(1.0f, 0.5f * mHand.timeVisible());
  mOpacity = confidenceMult * timeVisibleMult;

  mSkinnedVboHands->update();
}

void RiggedHand::MakeAdditionalModelViewTransformations(ModelView& model_view) const {

}

void RiggedHand::DrawContents(RenderState& renderState) const {
  glEnable(GL_TEXTURE_2D);

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
      glActiveTexture(GL_TEXTURE0);
      mSkinTex->Bind();
      glActiveTexture(GL_TEXTURE1);
      mNormalTex->Bind();
      glActiveTexture(GL_TEXTURE2);
      mSpecularTex->Bind();
      Color rimColor = mUseRim ? Color(0.075f, 0.1f, 0.125f, 1.0f) : Color::Black();
      Color specularColor = Color(mSpecular, mSpecular, mSpecular, 1.0f);
      Color ambientColor = Color(mAmbient, mAmbient, mAmbient, 1.0f);
      Color diffuseColor = Color(mDiffuse, mDiffuse, mDiffuse, 1.0f);

      mHandsShader->Bind();

      const Eigen::Matrix4f modelView =  renderState.GetModelView().Matrix().cast<float>().eval();
      mHandsShader->SetUniformMatrixf<4, 4, Eigen::Matrix4f>("modelView", modelView, MatrixStorageConvention::COLUMN_MAJOR);
      const Eigen::Matrix4f projection = renderState.GetProjection().Matrix().cast<float>().eval();
      mHandsShader->SetUniformMatrixf<4, 4, Eigen::Matrix4f>("projection", projection, MatrixStorageConvention::COLUMN_MAJOR);
      const Eigen::Matrix4f normalMatrix = renderState.GetModelView().Matrix().inverse().transpose().cast<float>().eval();
      mHandsShader->SetUniformMatrixf<4, 4, Eigen::Matrix4f>("normalMatrix", normalMatrix, MatrixStorageConvention::COLUMN_MAJOR);

      mHandsShader->SetUniformi("isAnimated", section->isAnimated());
      mHandsShader->SetUniformi("texture", 0);
      mHandsShader->SetUniformi("useNormalMap", mUseNormalMap);
      mHandsShader->SetUniformi("normalMap", 1);
      mHandsShader->SetUniformi("useSpecularMap", mUseSpecularMap);
      mHandsShader->SetUniformi("specularMap", 2);
      mHandsShader->SetUniformf("rimColor", rimColor);
      mHandsShader->SetUniformf("rimStart", mRimStart);
      mHandsShader->SetUniformf("innerTransparency", 1.0f-mOpacity);
      mHandsShader->SetUniformf("camPos", Eigen::Vector3f::Zero().eval());
      mHandsShader->SetUniformf("opacity", std::min(1.0f, 2.0f*mOpacity));

      mHandsShader->SetUniformf("lightPos", mLightPos);
      mHandsShader->SetUniformf("diffuse", diffuseColor);
      mHandsShader->SetUniformf("specular", specularColor);
      mHandsShader->SetUniformf("ambient", ambientColor);
      mHandsShader->SetUniformf("emission", Color::Black());
      mHandsShader->SetUniformf("shininess", mShininess);

      if (section->hasSkeleton()) {
        const int boneMatricesAddr = mHandsShader->LocationOfUniform("boneMatrices[0]");
        const int invTransposeMatricesAddr = mHandsShader->LocationOfUniform("invTransposeMatrices[0]");

        glUniformMatrix4fv(boneMatricesAddr, model::SkinnedVboMesh::MAXBONES, false, section->mBoneMatricesPtr->data()->data());
        glUniformMatrix4fv(invTransposeMatricesAddr, model::SkinnedVboMesh::MAXBONES, false, section->mInvTransposeMatricesPtr->data()->data());
      }

      const int positionAddr = mHandsShader->LocationOfAttribute("position");
      const int normalAddr = mHandsShader->LocationOfAttribute("normal");
      const int texcoordAddr = mHandsShader->LocationOfAttribute("texcoord");
      const int boneWeightsAddr = mHandsShader->LocationOfAttribute("boneWeights");
      const int boneIndicesAddr = mHandsShader->LocationOfAttribute("boneIndices");

      auto locations = std::make_tuple(positionAddr, normalAddr, texcoordAddr, boneWeightsAddr, boneIndicesAddr);
      section->getVboMesh().Enable(locations);

      const int numIndices = section->getIndices().Size();
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

    glActiveTexture(GL_TEXTURE2);
    mSpecularTex->Unbind();

    glActiveTexture(GL_TEXTURE1);
    mNormalTex->Unbind();

    glActiveTexture(GL_TEXTURE0);
    mSkinTex->Unbind();
  }
  if (mEnableWireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  glDisable(GL_TEXTURE_2D);
}

void RiggedHand::updateStyle() {
  // load new mesh if necessary
  if (mGender != mPrevGender) {
    mSkinnedVboHands = getMeshForGender(mGender);
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

void RiggedHand::updateLeapData() {
  const bool isLeft = mHand.isLeft();

  if (isLeft != mPrevIsLeft) {
    mArmReorientation = computeArmReorientation(isLeft);
    mWristReorientation = computeWristReorientation(isLeft);
    mFingerReorientation = computeFingerReorientation(isLeft);
    mPrevIsLeft = isLeft;
  }

  const Eigen::Matrix3d armBasis = toEigen(mHand.arm().basis());

  const Eigen::Matrix3d handBasis = toEigen(mHand.basis());
  const Eigen::Vector3f handDirection = mHand.direction().toVector3<Eigen::Vector3f>();
  const float palmWidth = mHand.palmWidth();

  mArmScale = mScaleMultiplier * palmWidth / 100;

  const Eigen::Vector3f handPos = 0.1f * mHand.palmPosition().toVector3<Eigen::Vector3f>();
  const Eigen::Vector3f wristPos = 0.1f * mHand.wristPosition().toVector3<Eigen::Vector3f>() - mArmScale*(handBasis.cast<float>()*mTranslationOffset);

  const Eigen::Vector3f forearmDir = mHand.arm().direction().toVector3<Eigen::Vector3f>();
  const Eigen::Vector3f forearmDirNormalized = forearmDir.normalized();
  mElbowPos = wristPos - mForearmLength * forearmDirNormalized;

  const Eigen::Quaterniond armQuat = toQuat(armBasis, mHand.isLeft());
  Eigen::Quaterniond handednessQuat = armQuat;
  if (isLeft) {
    handednessQuat.y() *= -1.0;
    handednessQuat.z() *= -1.0;
  }
  mArmRotation = handednessQuat * mArmReorientation;

  mHandRotation = toQuat(handBasis, mHand.isLeft());

  Eigen::Quaterniond rotationOffset = mRotationOffset.cast<double>();
  if (isLeft) {
    rotationOffset.x() *= -1.0;
    rotationOffset.y() *= -1.0;
  }

  mWristRotation = mWristReorientation * rotationOffset * armQuat.inverse() * mHandRotation;
  std::swap(mWristRotation.x(), mWristRotation.z());
  mWristRotation.x() *= -1;
  if (!isLeft) {
    mWristRotation.y() *= -1;
    mWristRotation.z() *= -1;
  }

  updateMeshMirroring(isLeft);
}

void RiggedHand::updateFinger(int fingerIdx) {
  const Leap::Finger finger = mHand.fingers()[fingerIdx];
  const bool isLeft = mHand.isLeft();
  Eigen::Quaterniond prevQuat = mHandRotation * mFingerReorientation;
  for (int boneIdx=0; boneIdx<3; boneIdx++) {
    const Leap::Bone bone = finger.bone(static_cast<Leap::Bone::Type>(boneIdx+1));
    const Eigen::Matrix3d boneBasis = toEigen(bone.basis());

    const Eigen::Quaterniond boneQuat = toQuat(boneBasis, isLeft) * mFingerReorientation;
    model::NodeRef boneNode = getJointNode(fingerIdx, boneIdx);
    boneNode->setRelativeRotation((prevQuat.inverse() * boneQuat).cast<float>());

    const int minBoneToAdjust = finger.type() == Leap::Finger::TYPE_THUMB ? 2 : 1;
    if (boneIdx >= minBoneToAdjust) {
      Leap::Bone prevBone = finger.bone(static_cast<Leap::Bone::Type>(boneIdx));
      Eigen::Vector3f relativePos = boneNode->getRelativePosition();
      const float relativePosLength = relativePos.norm();
      relativePos = 0.1f * prevBone.length() * (relativePos / relativePosLength) / mArmScale;
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

void RiggedHand::drawLeapHand(const Leap::Hand& hand) const {
  //if (mDrawLeap) {
  //  ci::gl::pushMatrices();
  //  glLineWidth(3.0f);
  //  glColor3f(0.4f, 0.9f, 1);
  //  for (int i=0; i<fingers.count(); i++) {
  //    Leap::Finger finger = fingers[i];
  //    ci::gl::drawSphere(0.1f * finger.tipPosition().toVector3<Eigen::Vector3f>(), 0.5f);
  //    for (int j=0; j<4; j++) {
  //      Leap::Bone bone = finger.bone(static_cast<Leap::Bone::Type>(j));
  //      glBegin(GL_LINES);
  //      Leap::Vector point1 = 0.1f * bone.prevJoint();
  //      Leap::Vector point2 = 0.1f * bone.nextJoint();
  //      glVertex3f(point1.x, point1.y, point1.z);
  //      glVertex3f(point2.x, point2.y, point2.z);
  //      glEnd();
  //    }
  //  }
  //  ci::gl::drawSphere(0.1f * hand.wristPosition().toVector3<Eigen::Vector3f>(), 1.0f);
  //  ci::gl::drawSphere(0.1f * hand.arm().elbowPosition().toVector3<Eigen::Vector3f>(), 1.0f);
  //  glBegin(GL_LINES);
  //  Leap::Vector point1 = 0.1f * hand.wristPosition();
  //  Leap::Vector point2 = 0.1f * hand.arm().elbowPosition();
  //  glVertex3f(point1.x, point1.y, point1.z);
  //  glVertex3f(point2.x, point2.y, point2.z);
  //  glEnd();
  //  ci::gl::popMatrices();
  //  glLineWidth(1.0f);
  //}
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


GLShaderRef RiggedHand::getHandsShader() {
  static GLShaderRef handsShader;
  if (handsShader == nullptr) {
    handsShader = Resource<GLShader>("hands");
  }
  return handsShader;
}

model::SkinnedVboMeshRef RiggedHand::getMeshForGender(Gender gender) {
  static model::ModelSourceRef maleMeshSource;
  static model::ModelSourceRef femaleMeshSource;
  if (gender == MALE) {
    if (maleMeshSource == nullptr) {
      maleMeshSource = model::loadModel("Male_Rigged_Arm.FBX");
    }
    return model::SkinnedVboMesh::create(maleMeshSource, nullptr, getHandsShader());
  } else if (gender == FEMALE) {
    if (femaleMeshSource == nullptr) {
      femaleMeshSource = model::loadModel("Female_Rigged_Arm.FBX");
    }
    return model::SkinnedVboMesh::create(femaleMeshSource, nullptr, getHandsShader());
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
    //rightHanded.setColumn(2, -1.0f * rightHanded.getColumn(2));
    rightHanded.col(2) *= -1.0;
  }
  static const Eigen::Vector3d LEAP_UP = Eigen::Vector3d::UnitY();
  static const Eigen::Vector3d LEAP_FORWARD = Eigen::Vector3d::UnitZ();
  const Eigen::Vector3d up = rightHanded * LEAP_UP;
  const Eigen::Vector3d forward = rightHanded * LEAP_FORWARD;
  return lookRotation(forward, up, leftHanded);
}

Eigen::Matrix3d RiggedHand::toEigen(const Leap::Matrix& mat) {
  Eigen::Matrix3d result;
  //result.col(0) = mat.xBasis.toVector3<Eigen::Vector3d>();
  //result.col(1) = mat.yBasis.toVector3<Eigen::Vector3d>();
  //result.col(2) = mat.zBasis.toVector3<Eigen::Vector3d>();
  result.col(0) << mat.xBasis.x, mat.xBasis.y, mat.xBasis.z;
  result.col(1) << mat.yBasis.x, mat.yBasis.y, mat.yBasis.z;
  result.col(2) << mat.zBasis.x, mat.zBasis.y, mat.zBasis.z;
  return result;
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
