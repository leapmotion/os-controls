#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include "HandActivationRecognizer.h"
#include "HandCursor.h"
#include "InteractionConfigs.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

enum class HandPose {
  ZeroFingers,
  OneFinger,
  TwoFingers,
  ThreeFingers,
  FourFingers,
  FiveFingers,
  UpsideDown
};

class HandPoseRecognizer {
public:
  HandPoseRecognizer(void);

  void AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, const HandPinch& handPinch, HandPose& handPose);
private:
  bool isUpsideDown(Leap::Hand hand);
  bool isExtended(Leap::Finger finger, bool wasExtended = false) const;
  float metaToDistalBend(Leap::Finger finger) const;
  float averageFingerBend(Leap::Finger finger, int startBone = 3, int endBone = 4) const;
  
  bool lastExtended [5];
  Eigen::Matrix<double,3,5> lastPosition;
  
  HandPose m_lastPose;
};