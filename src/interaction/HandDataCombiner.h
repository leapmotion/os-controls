#pragma once
#include "Leap.h"
#include "HandLocationRecognizer.h"
#include "HandActivationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "HandRollRecognizer.h"
#include "TimeRecognizer.h"

struct HandData {
  HandLocation locationData;
  HandPose handPose;
  HandRoll rollData;
  HandPinch pinchData;
  HandGrab grabData;
  double timeVisible;
};

class HandDataCombiner {
public:
  HandDataCombiner();
  ~HandDataCombiner();
  
  void AutoFilter(const HandLocation& handLocation, const HandTime& handTime, const HandPose& handPose, const HandRoll& handRoll, const HandPinch& handPinch, const HandGrab& handGrab, HandData& handData);
};