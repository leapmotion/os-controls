#include "RiggedHandLayer.h"

#include "Leap/GL/GLShader.h"
#include "ModelIo.h"
#include "ModelSourceAssimp.h"

#include <cmath>

RiggedHandLayer::RiggedHandLayer()
:
  m_Width(640),
  m_Height(480),
  m_time(0)
{
  mController.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  mPauseLeap = false;
}

RiggedHandLayer::~RiggedHandLayer() {

}

void RiggedHandLayer::Update(TimeDelta real_time_delta) {
  m_time += real_time_delta;

  if (!mPauseLeap) {
    Leap::Frame frame = mController.frame();
    if (mCurFrame != frame) {
      mCurFrame = frame;
      const int prevNumHands = static_cast<int>(mRiggedHands.size());
      const Leap::HandList hands = mCurFrame.hands();
      const int numHands = hands.count();
      mRiggedHands.resize(numHands);
      for (int i=prevNumHands; i<numHands; i++) {
        mRiggedHands[i] = RiggedHandRef(new RiggedHand());
        mRiggedHands[i]->SetStyle(RiggedHand::MALE, RiggedHand::MEDIUM);
      }
      for (int i=0; i<numHands; i++) {
        setRiggedHandFromLeapHand(mRiggedHands[i], hands[i]);
        mRiggedHands[i]->UpdateRigAndSkin();
      }
    }
  }
}

void RiggedHandLayer::Render(TimeDelta real_time_delta) const {
  glEnable(GL_DEPTH_TEST);

  // set renderer projection matrix
  const double fovRadians = (M_PI / 180.0) * 70;
  const double widthOverHeight = static_cast<double>(m_Width)/static_cast<double>(m_Height);
  const double nearClip = 10.0;
  const double farClip = 10000.0;
  m_Renderer.GetProjection().Perspective(fovRadians, widthOverHeight, nearClip, farClip);

  // set renderer modelview matrix
  const EigenTypes::Vector3 eyePos = EigenTypes::Vector3(0.0f, 300.0f, 250.0f);
  const EigenTypes::Vector3 lookAtPoint = EigenTypes::Vector3(0.0f, 150.0f, 0.0f);
  const EigenTypes::Vector3 upVector = EigenTypes::Vector3::UnitY();
  
  m_Renderer.GetModelView().Reset();
  m_Renderer.GetModelView().LookAt(eyePos, lookAtPoint, upVector);

  glEnable(GL_BLEND);

  Box centerBox;
  centerBox.SetSize(Eigen::Vector3d(10, 10, 10));
  PrimitiveBase::DrawSceneGraph(centerBox, m_Renderer);
  
  Box xBox;
  xBox.SetSize(Eigen::Vector3d(30, 1, 1));
  xBox.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(1.0f, 0.0f, 0.0f, 1.0f); // Red
  xBox.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(1.0f, 0.0f, 0.0f, 1.0f); // Red
  xBox.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.5f;
  xBox.Translation() = 15 * Eigen::Vector3d::UnitX();
  PrimitiveBase::DrawSceneGraph(xBox, m_Renderer);

  Box yBox;
  yBox.SetSize(Eigen::Vector3d(1, 30, 1));
  yBox.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(0.0f, 1.0f, 0.0f, 1.0f); // Green
  yBox.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.0f, 1.0f, 0.0f, 1.0f); // Green
  yBox.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.5f;
  yBox.Translation() = 15 * Eigen::Vector3d::UnitY();
  PrimitiveBase::DrawSceneGraph(yBox, m_Renderer);

  Box zBox;
  zBox.SetSize(Eigen::Vector3d(1, 1, 30));
  zBox.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Rgba<float>(0.0f, 0.0f, 1.0f, 1.0f); // Blue
  zBox.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Rgba<float>(0.0f, 0.0f, 1.0f, 1.0f); // Blue
  zBox.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.5f;
  zBox.Translation() = 15 * Eigen::Vector3d::UnitZ();
  PrimitiveBase::DrawSceneGraph(zBox, m_Renderer);

  for (size_t i=0; i<mRiggedHands.size(); i++) {
    if (!mRiggedHands[i]->HandsShader()) {
      mRiggedHands[i]->SetHandsShader(RiggedHand::getDefaultHandsShader());
    }

    std::shared_ptr<GLShader> shader = mRiggedHands[i]->HandsShader();
    shader->Bind();

    const float lightPos[] ={ -200, 200, -200, 200, 200, -200, 0, 100, 100 };
    const float lightStrengths[] ={ 1.0f, 1.0f, 1.0f };

    // const int lightPos0 = shader->LocationOfUniform("lightPos[0]");
    const int lightPos0 = shader->LocationOfUniform("lightPos");
    glUniform3fv(lightPos0, 3, lightPos); // TODO: replace with GLShader::UploadUniform

    // const int lightStrengths0 = shader->LocationOfUniform("lightStrengths[0]");
    const int lightStrengths0 = shader->LocationOfUniform("lightStrengths");
    glUniform1fv(lightStrengths0, 3, lightStrengths); // TODO: replace with GLShader::UploadUniform

    shader->UploadUniform<GL_INT>("numLights", 3);
    shader->UploadUniform<GL_FLOAT>("attenuation", 1.0f);
    shader->UploadUniform<GL_BOOL>("depthImage", false);
    shader->UploadUniform<GL_BOOL>("normalImage", false);
    shader->UploadUniform<GL_BOOL>("flatImage", false);
    shader->UploadUniform<GL_FLOAT_VEC4>("emission", Rgba<float>(0.0f, 0.0f, 0.0f, 1.0f)); // Opaque black
    shader->UploadUniform<GL_FLOAT>("minDepthDist", static_cast<float>(300));
    shader->UploadUniform<GL_FLOAT>("maxDepthDist", static_cast<float>(1000));
    shader->Unbind();
  }

  glEnable(GL_TEXTURE_2D);
  for (size_t i=0; i<mRiggedHands.size(); i++) {
    PrimitiveBase::DrawSceneGraph(*mRiggedHands[i], m_Renderer);
  }
  glDisable(GL_TEXTURE_2D);
}


Eigen::Matrix3d RiggedHandLayer::toEigen(const Leap::Matrix& mat) {
  Eigen::Matrix3d result;
  result.col(0) << mat.xBasis.x, mat.xBasis.y, mat.xBasis.z;
  result.col(1) << mat.yBasis.x, mat.yBasis.y, mat.yBasis.z;
  result.col(2) << mat.zBasis.x, mat.zBasis.y, mat.zBasis.z;
  return result;
}

void RiggedHandLayer::setRiggedHandFromLeapHand(RiggedHandRef riggedHand, Leap::Hand leapHand) {
  riggedHand->SetConfidence(leapHand.confidence());
  riggedHand->SetTimeVisible(leapHand.timeVisible());
  riggedHand->SetIsLeft(leapHand.isLeft());

  riggedHand->SetArmBasis(toEigen(leapHand.arm().basis()).cast<float>());
  riggedHand->SetHandBasis(toEigen(leapHand.basis()).cast<float>());
  riggedHand->SetPalmWidth(leapHand.palmWidth());
  riggedHand->SetWristPosition(leapHand.wristPosition().toVector3<Eigen::Vector3f>());

  const Leap::FingerList fingers = leapHand.fingers();
  for (int i=0; i<5; i++) {
    const Leap::Finger finger = fingers[i];
    for (int j=0; j<4; j++) {
      const Leap::Bone bone = finger.bone(static_cast<Leap::Bone::Type>(j));
      riggedHand->SetBoneLength(i, j, bone.length());
      riggedHand->SetBoneBasis(i, j, toEigen(bone.basis()).cast<float>());
    }
  }
}
