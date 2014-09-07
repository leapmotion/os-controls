#pragma once

#include "LeapListener.h"
#include "Primitives.h"
#include "Color.h"

struct HandBools {
  bool extended [5] = {};
};

class HandCursor : public PrimitiveBase {
public:
  HandCursor();

  void Update(const Leap::Hand& hand, HandBools ext);
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
  float averageFingerBend(Leap::Finger finger) const;
  
  void formatFinger(const Leap::Finger& finger, float distance, bool isLeft = false, bool isVisible = true);

  Color m_OutlineColor;
  Color m_FillColorOpen;
  Color m_FillColorClosed;
  Color m_fingerColor;

  float m_PalmOutlineRadius;
  float m_PalmOutlineThickness;

  std::shared_ptr<PartialDisk> m_PalmOutline;
  std::shared_ptr<Disk> m_PalmCenter;
  std::shared_ptr<Disk> m_Fingers[5];

};
