#pragma once
#include "Leap.h"
#include "EigenTypes.h"
#include "Animation.h"
#include <chrono>

struct Scroll {
  EigenTypes::Vector3 m_deltaScrollMM;
};

class ScrollRecognizer {
public:
  ScrollRecognizer();
  void AutoFilter(const Leap::Hand& hand, Scroll& scroll);

private:
  void ExtractFrameData();
  void UpdateHorizontalMovementRatio();
  float ComputeWarmupMultiplier() const;
  EigenTypes::Vector3 ComputeRoundedHandVelocity() const;
  void UpdateScrollVelocity();
  void AccumulateScrollFromFingers();

  static float DeadZoneMultiplier(const EigenTypes::Vector3& velocity);

  Smoothed<float> m_horizontalMovementRatio;

  Smoothed<EigenTypes::Vector3> m_scrollVelocity;
  EigenTypes::Vector3 m_curScrollVelocity;

  Leap::Hand m_hand;
  EigenTypes::Vector3 m_handVelocity;
  EigenTypes::Vector3 m_handDirection;
  EigenTypes::Vector3 m_handNormal;

  std::chrono::microseconds m_prevTimestamp;
  std::chrono::duration<float> m_deltaTimeSeconds;
};
