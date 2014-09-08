#include "HandCursor.h"

#include <algorithm>

HandCursor::HandCursor() {
  m_PalmOutline = std::shared_ptr<PartialDisk>(new PartialDisk());

  m_PalmCenter = std::shared_ptr<Disk>(new Disk());
  
  for (int i=0; i<NUM_FINGERS; i++) {
    m_Fingers[i] = std::shared_ptr<Disk>(new Disk());
  }
  
  m_OutlineColor = Color(0.802f, 0.802f, 0.802f, 0.75f); // 26 26 26
  m_FillColorClosed = Color(0.505f, 0.831f, 0.114f, 0.75f); // 129 212 29
  m_FillColorOpen = m_FillColorClosed;
  m_FillColorOpen.A() = 0.5f;
  
  Translation().z() = 0;
}

void HandCursor::InitChildren() {
  AddChild(m_PalmOutline);
  AddChild(m_PalmCenter);
  
  for (int i=0; i<NUM_FINGERS; i++) {
    AddChild(m_Fingers[i]);
  }

}

void HandCursor::Update(const Leap::Hand& hand) {
  const Leap::Vector handPos = transformCoordinates(hand.palmPosition());

  const float grabStrength = hand.grabStrength();
  const float pinchStrength = hand.pinchStrength();
  const float palmClosed = std::max(grabStrength, pinchStrength);

  // process fingers
  const Leap::FingerList fingers = hand.fingers();
  for (int i=0; i<NUM_FINGERS; i++) {
    const Leap::Finger finger = fingers[i];
    Vector3 fingerPosFlat = finger.tipPosition().toVector3<Vector3>();
    fingerPosFlat.z() = 0;
    Vector3 palmPosFlat = hand.palmPosition().toVector3<Vector3>();
    palmPosFlat.z() = 0;
    float bend = averageFingerBend(finger, 1, 2);
    
    if (finger.type() == Leap::Finger::TYPE_THUMB  ) {
      if ( finger.isExtended() ) {
        bend = 0.0f;
      }
      else {
        bend = 1.0f;
      }
    }
  
    formatFinger(finger, bend, hand.isLeft());
  }

  // process palm

  m_PalmOutlineRadius = 35.0f;
  m_PalmOutlineThickness = 1.0f;

  const auto handVec = handPos.toVector3<Vector3>();
  Translation().x() = handVec.x();
  Translation().y() = handVec.y();

  //LinearTransformation() = Eigen::AngleAxis<double>(M_PI/2.0, Vector3::UnitX()).toRotationMatrix();

  const Color palmCenterColor = calculatePalmColor(palmClosed);
  GLMaterial& palmCenterMat = m_PalmCenter->Material();
  palmCenterMat.SetAmbientLightColor(palmCenterColor);
  palmCenterMat.SetDiffuseLightColor(palmCenterColor);
  palmCenterMat.SetAmbientLightingProportion(1.0f);
  m_PalmCenter->SetRadius(30.0f);

  GLMaterial& palmOutlineMat = m_PalmOutline->Material();
  palmOutlineMat.SetAmbientLightColor(m_OutlineColor);
  palmOutlineMat.SetDiffuseLightColor(m_OutlineColor);
  palmOutlineMat.SetAmbientLightingProportion(1.0f);
  m_PalmOutline->SetInnerRadius(m_PalmOutlineRadius - m_PalmOutlineThickness/2.0f);
  m_PalmOutline->SetOuterRadius(m_PalmOutlineRadius + m_PalmOutlineThickness/2.0f);
  m_PalmOutline->SetStartAngle(0.0f);
  m_PalmOutline->SetEndAngle(2*M_PI);
}

void HandCursor::formatFinger(const Leap::Finger& finger, float bend, bool isLeft) {
  const float FINGER_REAL_BEND_MIN = 0.1f;
  const float FINGER_REAL_BEND_MAX = 0.7f;
  const float FINGER_DISTANCE_MIN = 35.0f;
  const float FINGER_DISTANCE_MAX = 50.0f;
  
  const float FINGER_MAX_SIZE = 5;
  const float FINGER_MIN_SIZE = 2.5;
  
  const Color FINGER_COLOR_OUT(0.505f, 0.831f, 0.114f, 0.75f);
  const Color FINGER_COLOR_IN(0.5f, 0.5f, 0.5f, 0.75f);
  
  const float angles [5] { M_PI/12.0f, M_PI/3.0f, M_PI/2.0f, 2*M_PI/3.0f, 5*M_PI/6.0f };
  int fingerIndex = static_cast<int>(finger.type());
  float angle = angles[fingerIndex];
  angle = (2*M_PI) - angle;
  if ( !isLeft ) {
    angle = (M_PI) - angle;
  }
  Vector2 visualPosition(std::cos(angle), std::sin(angle));
  
  float bendNorm = (bend - FINGER_REAL_BEND_MIN) / (FINGER_REAL_BEND_MAX - FINGER_REAL_BEND_MIN);
  bendNorm = 1.0f - std::min(1.0f, std::max(0.0f, bendNorm));
  float visualDist = FINGER_DISTANCE_MIN + (bendNorm*(FINGER_DISTANCE_MAX - FINGER_DISTANCE_MIN));
  
  const Vector4f blend = (bendNorm * FINGER_COLOR_OUT.Data()) + ((1.0f-bendNorm) * FINGER_COLOR_IN.Data());
  Color blendedColor(blend);
  
  visualPosition *= visualDist;

  std::shared_ptr<Disk> fingerVisual = m_Fingers[fingerIndex];
  fingerVisual->Translation() = Vector3(visualPosition.x(), visualPosition.y(), fingerVisual->Translation().z());
  fingerVisual->SetRadius( FINGER_MIN_SIZE + (bendNorm * (FINGER_MAX_SIZE - FINGER_MIN_SIZE)) );
  
  fingerVisual->Material().SetAmbientLightColor(blendedColor);
  fingerVisual->Material().SetDiffuseLightColor(blendedColor);
  fingerVisual->Material().SetAmbientLightingProportion(1.0f);
}

void HandCursor::Draw(RenderState& renderState) const {
  // nothing to do, our children will be drawn automatically
}

Leap::Vector HandCursor::transformCoordinates(const Leap::Vector& vec) {
  static const Leap::Vector LEAP_OFFSET(0.0f, -200.0f, 0.0f);
  return vec + LEAP_OFFSET;
}

Leap::Vector HandCursor::rotateCoordinates(const Leap::Vector& vec) {
  return Leap::Vector(vec.x, -vec.z, vec.y);
}

float HandCursor::calculateFingerBend(const Leap::Finger& finger) {
#if 0
  return finger.isExtended() ? 0.0f : 1.0f;
#else
  const float grabStrength = finger.hand().grabStrength();
  return grabStrength * grabStrength;
#endif
}

float HandCursor::calculatePalmFillRadius(float closed) {
#if 0
  static const float MIN_RADIUS = 0.0f;
  static const float MAX_RADIUS = 34.5f;
#else
  static const float MIN_RADIUS = 34.5f;
  static const float MAX_RADIUS = 0.0f;
#endif
  return (1.0f-closed)*MIN_RADIUS + closed*MAX_RADIUS;
}

float HandCursor::calculateFingerRadius(float bend) {
  //static const float MIN_RADIUS = 2.5f;
  static const float MIN_RADIUS = 27.5f;
  static const float MAX_RADIUS = 5.0f;
  return (1.0f-bend)*MAX_RADIUS + bend*MIN_RADIUS;
}

Color HandCursor::calculateFingerColor(float bend) {
#if 0
  const Vector4f blended = bend*m_OutlineColor.Data() + (1.0f-bend)*m_FillColorClosed.Data();
  return Color(blended);
#else
  return m_FillColorClosed;
#endif
}

Color HandCursor::calculatePalmColor(float closed) {
  
  //const Vector4f blended = closed*m_FillColorClosed.Data() + (1.0f-closed)*m_FillColorOpen.Data();
  return Color(m_FillColorClosed);
}

float HandCursor::averageFingerBend(Leap::Finger finger, int startBone, int endBone) const {
  float retVal = 0.0f;
  int count = 0;
  float sum = 0.0f;
  float average = 0.0f;
  startBone = std::max(1, startBone);
  endBone = std::min(4, endBone);
  
  for(int i=startBone; i<endBone; i++) {
    //Angle from scalar product
    Vector3 v1 = finger.bone(static_cast<Leap::Bone::Type>(i-1)).direction().toVector3<Vector3>();
    Vector3 v2 = finger.bone(static_cast<Leap::Bone::Type>(i)).direction().toVector3<Vector3>();
    double dot = v1.dot(v2);
    double theta = std::acos(dot);
    sum += static_cast<float>(theta);
    count++;
  }
  average = count > 0 ? sum / count : 0.0f;
  
  retVal = average;
  return retVal;
}
