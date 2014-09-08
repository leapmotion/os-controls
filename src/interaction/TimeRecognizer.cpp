#include "stdafx.h"
#include "TimeRecognizer.h"

TimeRecognizer::TimeRecognizer() :
lastTimestamp(-1) {
  
}

void TimeRecognizer::AutoFilter(const Leap::Frame& frame, const Leap::Hand hand, HandTime& handTime, FrameTime& frameTime) {
  if (lastTimestamp == -1) { lastTimestamp = frame.timestamp(); }
  frameTime.deltaTime = frame.timestamp() - lastTimestamp;
  lastTimestamp = frame.timestamp();
  handTime.timeVisible = hand.timeVisible();
}