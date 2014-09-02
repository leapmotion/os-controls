#pragma once
#include "Leap.h"

struct FrameTime {
  int64_t deltaTime;
};

class FrameDeltaTimeRecognizer {
public:
  FrameDeltaTimeRecognizer();
  virtual ~FrameDeltaTimeRecognizer() {};
  
  void AutoFilter(const Leap::Frame& frame, FrameTime& frameTime);
  
private:
  int64_t lastTimestamp;
};