#pragma once
#include "Leap.h"
#include "TimeRecognizer.h"

enum class HandPose;

struct HandPinch {
  bool isPinching;
  float pinchStrength;
  float pinchDeltaPerSecond;
};

class HandPinchRecognizer {
public:
  HandPinchRecognizer();
  virtual~HandPinchRecognizer(){};
  
  void AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, HandPinch& handPinch);
private:
  
  float getCustomPinchStrength(const Leap::Hand& hand) const;
  float calcHandPinchVelocity(const Leap::Finger& index, const Leap::Finger& thumb) const;
  
  bool m_wasPinching;
  float m_lastPinchStrength;
};