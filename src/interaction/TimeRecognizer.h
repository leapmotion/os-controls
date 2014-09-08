#pragma once
#include "Leap.h"

struct FrameTime {
  int64_t deltaTime;
};

struct HandTime {
  double timeVisible;
};

class TimeRecognizer {
public:
  TimeRecognizer();
  virtual ~TimeRecognizer() {};
  
  void AutoFilter(const Leap::Frame& frame, const Leap::Hand hand, HandTime& handTime, FrameTime& frameTime);
  
private:
  int64_t lastTimestamp;
};