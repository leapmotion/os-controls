#include "HandCursor.h"

#include <algorithm>

HandCursor::HandCursor() {
  m_PalmOutline = std::shared_ptr<PartialDisk>(new PartialDisk());

  m_PalmCenter = std::shared_ptr<Disk>(new Disk());
  
  for (int i=0; i<NUM_FINGERS; i++) {
    m_Fingers[i] = std::shared_ptr<Disk>(new Disk());
  }
  
  m_OutlineColor = Color(0.802f, 0.802f, 0.802f, 1.0f); // 26 26 26
  m_FillColor = Color(0.505f, 0.831f, 0.114f, 1.0f); // 129 212 29
  
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
  
  float palmNorm = (palmClosed - PALM_MIN_ACT) / (PALM_MAX_ACT - PALM_MIN_ACT);
  palmNorm = std::min(1.0f, std::max(0.0f, palmNorm));
  
  float palmRadius = PALM_MIN_RAD + (palmNorm*(PALM_MAX_RAD - PALM_MIN_RAD));

  GLMaterial& palmCenterMat = m_PalmCenter->Material();
  palmCenterMat.SetAmbientLightColor(m_FillColor);
  palmCenterMat.SetDiffuseLightColor(m_FillColor);
  palmCenterMat.SetAmbientLightingProportion(1.0f);
  m_PalmCenter->SetRadius(palmRadius);

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
  
  const Color FINGER_COLOR_OUT(0.505f, 0.831f, 0.114f, 1.0f);
  const Color FINGER_COLOR_IN(0.5f, 0.5f, 0.5f, 1.0f);
  
  const float angles [5] { M_PI/12.0f, M_PI/3.0f, M_PI/2.0f, 2*M_PI/3.0f, 5*M_PI/6.0f };
  int fingerIndex = static_cast<int>(finger.type());
  float angle = angles[fingerIndex];
  angle = static_cast<float>((2*M_PI) - angle);
  if ( !isLeft ) {
    angle = static_cast<float>(M_PI - angle);
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

void HandCursor::DrawContents(RenderState& renderState) const {
  // nothing to do, our children will be drawn automatically
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
