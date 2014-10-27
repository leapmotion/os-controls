#include "RiggedHandLayer.h"

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
        mRiggedHands[i]->SetStyle(RiggedHand::FEMALE, RiggedHand::MEDIUM);
      }
      for (int i=0; i<numHands; i++) {
        mRiggedHands[i]->SetLeapHand(hands[i]);
        mRiggedHands[i]->UpdateRigAndSkin();
      }
    }
  }
}

void RiggedHandLayer::Render(TimeDelta real_time_delta) const {
  glEnable(GL_DEPTH_TEST);

  // set renderer projection matrix
  const double fovRadians = (M_PI / 180.0) * 80;
  const double widthOverHeight = static_cast<double>(m_Width)/static_cast<double>(m_Height);
  const double nearClip = 10.0;
  const double farClip = 10000.0;
  m_Renderer.GetProjection().Perspective(fovRadians, widthOverHeight, nearClip, farClip);

  // set renderer modelview matrix
  const EigenTypes::Vector3 eyePos = EigenTypes::Vector3(0.0f, 350.0f, 300.0f);
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
  xBox.Material().SetDiffuseLightColor(Color::Red());
  xBox.Material().SetAmbientLightColor(Color::Red());
  xBox.Translation() = 15 * Eigen::Vector3d::UnitX();
  PrimitiveBase::DrawSceneGraph(xBox, m_Renderer);

  Box yBox;
  yBox.SetSize(Eigen::Vector3d(1, 30, 1));
  yBox.Material().SetDiffuseLightColor(Color::Green());
  yBox.Material().SetAmbientLightColor(Color::Green());
  yBox.Translation() = 15 * Eigen::Vector3d::UnitY();
  PrimitiveBase::DrawSceneGraph(yBox, m_Renderer);

  Box zBox;
  zBox.SetSize(Eigen::Vector3d(1, 1, 30));
  zBox.Material().SetDiffuseLightColor(Color::Blue());
  zBox.Material().SetAmbientLightColor(Color::Blue());
  zBox.Translation() = 15 * Eigen::Vector3d::UnitZ();
  PrimitiveBase::DrawSceneGraph(zBox, m_Renderer);

  glEnable(GL_TEXTURE_2D);
  for (size_t i=0; i<mRiggedHands.size(); i++) {
    PrimitiveBase::DrawSceneGraph(*mRiggedHands[i], m_Renderer);
  }
  glDisable(GL_TEXTURE_2D);
}
