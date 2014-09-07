#pragma once
#include "Leap.h"
#include "EigenTypes.h"
#include "Animation.h"
#include <chrono>

struct Scroll {
  Vector3 m_deltaScrollMM;
};

class ScrollRecognizer {
public:
  ScrollRecognizer();
  void AutoFilter(const Leap::Hand& hand, Scroll& scroll);

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

  std::chrono::microseconds m_prevTimestamp;
  std::chrono::duration<float> m_deltaTimeSeconds;
};
