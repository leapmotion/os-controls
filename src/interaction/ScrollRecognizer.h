#pragma once

#include "Leap.h"
#include "EigenTypes.h"
//#include <map>
#include "Animation.h"

#include <autowiring/Autowired.h>

struct Scroll {
  Vector3 m_deltaScrollMM;
};

class ScrollRecognizer {
public:
  ScrollRecognizer();
  void AutoFilter(const Leap::Hand& hand, Scroll& scroll);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:

  void ExtractFrameData();
  void UpdateHorizontalMovementRatio();
  float ComputeWarmupMultiplier() const;
  Vector3 ComputeRoundedHandVelocity() const;
  void UpdateScrollVelocity();
  void AccumulateScrollFromFingers();

  static float DeadZoneMultiplier(const Vector3& velocity);

  Smoothed<float> m_horizontalMovementRatio;
  Smoothed<Vector3> m_scrollVelocity;

  Vector3 m_curScrollVelocity;
  
  Leap::Hand m_hand;
  Vector3 m_handVelocity;
  Vector3 m_handDirection;
  Vector3 m_handNormal;

  int64_t m_prevTimestamp;
  float m_deltaTimeSeconds;
};
