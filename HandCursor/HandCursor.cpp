#include "HandCursor.h"

#include <algorithm>

HandCursor::HandCursor() {
  m_PalmOutline = std::shared_ptr<PartialDisk>(new PartialDisk());
  //AddChild(m_PalmOutline);

  m_PalmCenter = std::shared_ptr<Disk>(new Disk());
  
  for (int i=0; i<NUM_FINGERS; i++) {
    m_Fingers[i] = std::shared_ptr<Disk>(new Disk());
  }
  
  m_OutlineColor = Color(0.102f, 0.102f, 0.102f, 1.0f); // 26 26 26
  m_FillColorClosed = Color(0.505f, 0.831f, 0.114f, 0.5f); // 129 212 29
  m_FillColorOpen = m_FillColorClosed;
  m_FillColorOpen.A() = 0.5f;
}

void HandCursor::InitChildren() {
  AddChild(m_PalmCenter);
  
  for (int i=0; i<NUM_FINGERS; i++) {
    AddChild(m_Fingers[i]);
  }

}

void HandCursor::Update(const Leap::Hand& hand) {
  static const double LOC_Z = -1000;

  const Leap::Vector handPos = transformCoordinates(hand.palmPosition());
  const Leap::Matrix handBasis = hand.basis();
  const Leap::Matrix handBasisInv = handBasis.rigidInverse();

  const float grabStrength = hand.grabStrength();
  const float pinchStrength = hand.pinchStrength();
  const float palmClosed = std::max(grabStrength, pinchStrength);

  // process fingers
  const Leap::FingerList fingers = hand.fingers();
  for (int i=0; i<NUM_FINGERS; i++) {
    const Leap::Finger finger = fingers[i];
    const float fingerBend = calculateFingerBend(finger);
    const float fingerRadius = calculateFingerRadius(fingerBend);
    const Leap::Vector relativeFingerPos = transformCoordinates(finger.tipPosition()) - handPos;
#if 0
    const Leap::Vector transformedFingerPos = rotateCoordinates(handBasisInv.transformPoint(relativeFingerPos));
#else
    const Leap::Vector transformedFingerPos = relativeFingerPos;
#endif
    const float closedFingerRadius = m_PalmOutlineRadius + m_PalmOutlineThickness/2.0f + fingerRadius;
#if 0
    const Leap::Vector closedFingerPos = closedFingerRadius * calculateClosedFingerPosition(i);
#else
    const Leap::Vector closedFingerPos = Leap::Vector::zero();
#endif


#if 1
    //const Leap::Vector blendedFingerPos = fingerBend*closedFingerPos + (1.0f-fingerBend)*transformedFingerPos;
    const Leap::Vector blendedFingerPos = grabStrength*closedFingerPos + (1.0f-grabStrength)*transformedFingerPos;
#else
    const Leap::Vector blendedFingerPos = transformedFingerPos;
#endif
    
    const Color fingerColor = calculateFingerColor(fingerBend);

    m_Fingers[i]->Translation() = blendedFingerPos.toVector3<Vector3>();
    m_Fingers[i]->Translation().z() = 0.0;
    m_Fingers[i]->SetRadius(fingerRadius);

    GLMaterial& fingerMat = m_Fingers[i]->Material();
    fingerMat.SetAmbientLightColor(fingerColor);
    fingerMat.SetDiffuseLightColor(fingerColor);
    fingerMat.SetAmbientLightingProportion(1.0f);
  }

  // process palm

  m_PalmOutlineRadius = 35.0f;
  m_PalmOutlineThickness = 2.0f;

  Translation() = handPos.toVector3<Vector3>();
  Translation().z() = LOC_Z;

  //LinearTransformation() = Eigen::AngleAxis<double>(M_PI/2.0, Vector3::UnitX()).toRotationMatrix();

  const float palmCenterRadius = calculatePalmFillRadius(palmClosed);

  const Color palmCenterColor = calculatePalmColor(palmClosed);
  GLMaterial& palmCenterMat = m_PalmCenter->Material();
  palmCenterMat.SetAmbientLightColor(palmCenterColor);
  palmCenterMat.SetDiffuseLightColor(palmCenterColor);
  palmCenterMat.SetAmbientLightingProportion(1.0f);
  m_PalmCenter->SetRadius(palmCenterRadius);

  GLMaterial& palmOutlineMat = m_PalmOutline->Material();
  palmOutlineMat.SetAmbientLightColor(m_OutlineColor);
  palmOutlineMat.SetDiffuseLightColor(m_OutlineColor);
  palmOutlineMat.SetAmbientLightingProportion(1.0f);
  m_PalmOutline->SetInnerRadius(m_PalmOutlineRadius - m_PalmOutlineThickness/2.0f);
  m_PalmOutline->SetOuterRadius(m_PalmOutlineRadius + m_PalmOutlineThickness/2.0f);
  m_PalmOutline->SetStartAngle(0.0f);
  m_PalmOutline->SetEndAngle(2*M_PI);
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

Leap::Vector HandCursor::calculateClosedFingerPosition(int fingerIdx) {
  static const float DEGREES_TO_RADIANS = static_cast<float>(M_PI / 180.0);
  static bool init = false;
  static Leap::Vector positions[NUM_FINGERS];
  //static float angles[] = { 33.0f, 59.0f, 90.0f, 121.0f, 172.0f };
  static float angles[] ={ 172.0f, 121.0f, 90.0f, 59.0f, 33.0f };
  if (!init) {
    for (int i=0; i<NUM_FINGERS; i++) {
      const float angleRadians = DEGREES_TO_RADIANS * angles[i];
      const float cartesianX = std::cos(angleRadians);
      const float cartesianY = std::sin(angleRadians);
      positions[i] = Leap::Vector(cartesianX, cartesianY, 0.0f);
    }
    init = true;
  }
  return positions[fingerIdx];
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
  const Vector4f blended = closed*m_FillColorClosed.Data() + (1.0f-closed)*m_FillColorOpen.Data();
  return Color(blended);
}
