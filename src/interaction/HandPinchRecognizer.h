#pragma once
#include "Leap.h"

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
  bool wasPinching;
};