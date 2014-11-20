#include "HandCursor.h"

#include <algorithm>

HandCursor::HandCursor() {
  m_PalmOutline = std::shared_ptr<PartialDisk>(new PartialDisk());

  m_PalmCenter = std::shared_ptr<Disk>(new Disk());

  m_PalmDropShadow = std::shared_ptr<DropShadow>(new DropShadow);
  
  for (int i=0; i<NUM_FINGERS; i++) {
    m_Fingers[i] = std::shared_ptr<Disk>(new Disk());
    m_FingerDropShadows[i] = std::shared_ptr<DropShadow>(new DropShadow());
  }

  m_OutlineColor = Color(1.0f, 1.0f, 1.0f, 1.0f); // 26 26 26
  m_FillColor = Color(0.505f, 0.831f, 0.114f, 1.0f); // 129 212 29
  
  Translation().z() = 0;

  m_DrawStyle = RINGED_PAW;
}

void HandCursor::InitChildren() {
  // add all shadows first
  for (int i=0; i<NUM_FINGERS; i++) {
    AddChild(m_FingerDropShadows[i]);
  }
  AddChild(m_PalmDropShadow);
  
  // add solid geometry
  for (int i=0; i<NUM_FINGERS; i++) {
    AddChild(m_Fingers[i]);
  }
  AddChild(m_PalmOutline);
  AddChild(m_PalmCenter);
}

void HandCursor::Update(const Leap::Hand& hand) {
  const float PALM_MAX_RAD = 30.0f;
  const float PALM_MIN_RAD = 20.0f;
  const float PALM_MAX_ACT = 0.9f;
  const float PALM_MIN_ACT = 0.3f;
  
  const float grabStrength = hand.grabStrength();
  const float pinchStrength = hand.pinchStrength();
  const float palmClosed = std::max(grabStrength, pinchStrength);

  // process fingers
  const Leap::FingerList fingers = hand.fingers();
  for (int i=0; i<NUM_FINGERS; i++) {
    const Leap::Finger finger = fingers[i];
    EigenTypes::Vector3 fingerPosFlat = finger.tipPosition().toVector3<EigenTypes::Vector3>();
    fingerPosFlat.z() = 0;
    EigenTypes::Vector3 palmPosFlat = hand.palmPosition().toVector3<EigenTypes::Vector3>();
    palmPosFlat.z() = 0;
    const float bend = fingerBend(finger);
    formatFinger(finger, bend, hand.isLeft());
  }
  
  float palmNorm = (palmClosed - PALM_MIN_ACT) / (PALM_MAX_ACT - PALM_MIN_ACT);
  palmNorm = std::min(1.0f, std::max(0.0f, palmNorm));
  
  const float palmRadius = PALM_MIN_RAD + (palmNorm*(PALM_MAX_RAD - PALM_MIN_RAD));

  // process palm
  m_PalmOutlineThickness = m_DrawStyle == BLOB ? (PALM_MAX_RAD / 4.0f) * palmNorm : 1.0f;
  m_PalmOutlineRadius = m_DrawStyle == BLOB ? palmRadius : 35.0f;

  auto &palmCenterMat = m_PalmCenter->Material();
  palmCenterMat.Uniform<AMBIENT_LIGHT_COLOR>() = m_FillColor;
  palmCenterMat.Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_PalmCenter->SetRadius(palmRadius);

  auto &palmOutlineMat = m_PalmOutline->Material();
  palmOutlineMat.Uniform<AMBIENT_LIGHT_COLOR>() = m_OutlineColor;
  palmOutlineMat.Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_PalmOutline->SetInnerRadius(m_PalmOutlineRadius - m_PalmOutlineThickness/2.0f);
  m_PalmOutline->SetOuterRadius(m_PalmOutlineRadius + m_PalmOutlineThickness/2.0f);
  m_PalmOutline->SetStartAngle(0.0f);
  m_PalmOutline->SetEndAngle(2*M_PI);

  static const EigenTypes::Vector3 DROP_SHADOW_OFFSET(2, 3, 0);
  const double DROP_SHADOW_RADIUS = 3.5*palmRadius;
  static const float DROP_SHADOW_OPACITY = 0.4f;
  m_PalmDropShadow->Translation() = DROP_SHADOW_OFFSET;
  m_PalmDropShadow->SetBasisRectangleSize(EigenTypes::Vector2::Zero());
  m_PalmDropShadow->SetShadowRadius(DROP_SHADOW_RADIUS);
  m_PalmDropShadow->LocalProperties().AlphaMask() = DROP_SHADOW_OPACITY;

  const EigenTypes::Vector3 palmVel = hand.palmVelocity().toVector3<EigenTypes::Vector3>().cwiseProduct(EigenTypes::Vector3(1, -1, 0));
  LinearTransformation() = PrimitiveBase::SquashStretchTransform(palmVel, EigenTypes::Vector3::UnitZ());
}

void HandCursor::formatFinger(const Leap::Finger& finger, float bend, bool isLeft) {
  static const float FINGER_REAL_BEND_MIN = 0.3f;
  static const float FINGER_REAL_BEND_MAX = 0.6f;
  const float FINGER_DISTANCE_MIN = m_DrawStyle == BLOB ? 0.0f : 35.0f;
  static const float FINGER_DISTANCE_MAX = 50.0f;

  static const float FINGER_SIZE_OUT = 8.0f;
  const float FINGER_SIZE_IN = m_DrawStyle == BLOB ? 20.0f : 2.5f;

  static const Color FINGER_COLOR_OUT(0.505f, 0.831f, 0.114f, 1.0f);
  const Color FINGER_COLOR_IN = m_DrawStyle == BLOB ? FINGER_COLOR_OUT : Color(0.5f, 0.5f, 0.5f, 1.0f);

  static const float M_PIf = static_cast<float>(M_PI);

  static const float relativeSizes[5] = { 1.0f, 0.85f, 1.0f, 0.8f, 0.65f };
  static const float relativeLengths[5] = { 0.85f, 0.975f, 1.0f, 0.975f, 0.9f };
  static const float angles[5] { M_PIf/12.0f, M_PIf/3.0f, M_PIf/2.0f, 2.0f*M_PIf/3.0f, 5.0f*M_PIf/6.0f };

  int fingerIndex = static_cast<int>(finger.type());
  float angle = angles[fingerIndex];
  angle = static_cast<float>((2*M_PI) - angle);
  if ( !isLeft ) {
    angle = static_cast<float>(M_PI - angle);
  }
  EigenTypes::Vector2 visualPosition(std::cos(angle), std::sin(angle));
  
  float bendNorm = (bend - FINGER_REAL_BEND_MIN) / (FINGER_REAL_BEND_MAX - FINGER_REAL_BEND_MIN);
  bendNorm = 1.0f - std::min(1.0f, std::max(0.0f, bendNorm));
  float visualDist = relativeLengths[fingerIndex] * (FINGER_DISTANCE_MIN + (bendNorm*(FINGER_DISTANCE_MAX - FINGER_DISTANCE_MIN)));
  
  const EigenTypes::Vector4f blend = (bendNorm * FINGER_COLOR_OUT.Data()) + ((1.0f-bendNorm) * FINGER_COLOR_IN.Data());
  Color blendedColor(blend);
  
  visualPosition *= visualDist;

  const double fingerRadius = relativeSizes[fingerIndex] * (FINGER_SIZE_IN + (bendNorm * (FINGER_SIZE_OUT - FINGER_SIZE_IN)));

  std::shared_ptr<Disk>& fingerVisual = m_Fingers[fingerIndex];
  const EigenTypes::Vector3 fingerTranslation(visualPosition.x(), visualPosition.y(), fingerVisual->Translation().z());
  fingerVisual->Translation() = fingerTranslation;
  fingerVisual->SetRadius(fingerRadius);
  
  fingerVisual->Material().Uniform<AMBIENT_LIGHT_COLOR>() = blendedColor;
  fingerVisual->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;

  std::shared_ptr<DropShadow>& fingerDropShadow = m_FingerDropShadows[fingerIndex];

  static const EigenTypes::Vector3 DROP_SHADOW_OFFSET(2, 3, 0);
  const double DROP_SHADOW_RADIUS = 3.5*fingerRadius;
  static const float DROP_SHADOW_OPACITY = 0.4f;
  fingerDropShadow->Translation() = fingerTranslation + DROP_SHADOW_OFFSET;
  fingerDropShadow->SetBasisRectangleSize(EigenTypes::Vector2::Zero());
  fingerDropShadow->SetShadowRadius(DROP_SHADOW_RADIUS);
  fingerDropShadow->LocalProperties().AlphaMask() = DROP_SHADOW_OPACITY;
}

void HandCursor::DrawContents(RenderState& renderState) const {
  // nothing to do, our children will be drawn automatically
}

float HandCursor::fingerBend(const Leap::Finger& finger) const {
  int count = 0;
  float sum = 0.0f;
  float average = 0.0f;
  int startBone = 2; // thumb doesn't have metacarpal base
  int endBone = 3;
  
  for(int i=startBone; i<endBone; i++) {
    //Angle from scalar product
    EigenTypes::Vector3 v1 = finger.bone(static_cast<Leap::Bone::Type>(i-1)).direction().toVector3<EigenTypes::Vector3>();
    EigenTypes::Vector3 v2 = finger.bone(static_cast<Leap::Bone::Type>(i)).direction().toVector3<EigenTypes::Vector3>();
    double dot = v1.dot(v2);
    double theta = std::acos(dot);
    sum += static_cast<float>(theta);
    count++;
  }
  average = count > 0 ? sum / count : 0.0f;

  const float mult = static_cast<float>(2.0/M_PI);
  if (finger.type() == Leap::Finger::TYPE_THUMB) {
    average = std::max(2.0f * average, finger.hand().pinchStrength());
  }
  return mult * average;
}
