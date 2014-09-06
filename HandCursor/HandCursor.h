#pragma once

#include "LeapListener.h"
#include "Primitives.h"
#include "Color.h"

class HandCursor : public PrimitiveBase {
public:
  HandCursor();

  void Update(const Leap::Hand& hand);
  void InitChildren();

  virtual void Draw(RenderState& renderState) const override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  static const int NUM_FINGERS = 5;

  static Leap::Vector transformCoordinates(const Leap::Vector& vec);
  static Leap::Vector rotateCoordinates(const Leap::Vector& vec);
  static Leap::Vector calculateClosedFingerPosition(int fingerIdx);
  static float calculateFingerBend(const Leap::Finger& finger);
  static float calculatePalmFillRadius(float closed);
  static float calculateFingerRadius(float bend);
  Color calculateFingerColor(float bend);
  Color calculatePalmColor(float closed);

  Color m_OutlineColor;
  Color m_FillColorOpen;
  Color m_FillColorClosed;

  float m_PalmOutlineRadius;
  float m_PalmOutlineThickness;
  //float m_PalmFillRadius;

  std::shared_ptr<PartialDisk> m_PalmOutline;
  std::shared_ptr<Disk> m_PalmCenter;
  std::shared_ptr<Disk> m_Fingers[5];

};
