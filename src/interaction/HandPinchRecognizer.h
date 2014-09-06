#pragma once
#include "Leap.h"

enum class HandPose;

struct HandPinch {
  bool isPinching;
  float pinchStrength;
};

class HandPinchRecognizer {
public:
  HandPinchRecognizer();
  virtual~HandPinchRecognizer(){};
  
  void AutoFilter(const Leap::Hand& hand, HandPinch& handPinch);
private:
  
  float getCustomPinchStrength(const Leap::Hand& hand) const;
  
  bool wasPinching;
};