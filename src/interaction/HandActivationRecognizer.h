#pragma once
#include "Leap.h"
#include "TimeRecognizer.h"
#include "interaction/interactionConfigs.h"

struct HandPinch {
  bool isPinching;
  float pinchStrength;
  float pinchDeltaPerSecond;
};

struct HandGrab {
  bool isGrabbing;
  float grabStrength;
  float grabDeltaPerSecond;
};

class HandActivationRecognizer {
public:
  HandActivationRecognizer();
  virtual~HandActivationRecognizer(){};
  
  void AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, HandGrab& handGrab, HandPinch& handPinch);
private:
  float getCustomPinchStrength(const Leap::Hand& hand) const;
  float calcHandPinchVelocity(const Leap::Finger& index, const Leap::Finger& thumb) const;
  
  bool m_wasPinching;
  float m_lastPinchStrength;
  
  bool m_wasGrabbing;
  float m_lastGrabStrength;
};