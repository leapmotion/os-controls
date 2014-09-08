#pragma once
#include "Leap.h"
#include "HandLocationRecognizer.h"
#include "HandPinchRecognizer.h"
#include "HandPoseRecognizer.h"
#include "HandRollRecognizer.h"
#include "TimeRecognizer.h"

struct HandData {
  HandLocation locationData;
  HandPose handPose;
  HandRoll rollData;
  HandPinch pinchData;
  double timeVisible;
};

class HandDataCombiner {
public:
  HandDataCombiner();
  ~HandDataCombiner();
  
  void AutoFilter(const HandLocation& handLocation, const HandTime& handTime, const HandPose& handPose, const HandRoll& handRoll, const HandPinch& handPinch, HandData& handData);
};