#include "stdafx.h"
#include "FrameDeltaTimeRecognizer.h"

FrameDeltaTimeRecognizer::FrameDeltaTimeRecognizer() :
lastTimestamp(-1) {
  
}

void FrameDeltaTimeRecognizer::AutoFilter(const Leap::Frame& frame, FrameTime& frameTime) {
  if (lastTimestamp == -1) { lastTimestamp = frame.timestamp(); }
  frameTime.deltaTime = frame.timestamp() - lastTimestamp;
  lastTimestamp = frame.timestamp();
}