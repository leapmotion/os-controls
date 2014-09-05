#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include "HandPinchRecognizer.h"
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
  Clawed
};

class HandPoseRecognizer {
public:
  HandPoseRecognizer(void);

  void AutoFilter(const Leap::Hand& frame, HandPose& handPose);
private:
  bool isExtended(Leap::Finger finger, bool wasExtended = false) const;
  bool isClawCurled(Leap::Finger finger) const;
  bool areTipsClawed(Leap::Hand hand) const;
  bool lastExtended [5];
  float averageFingerBend(Leap::Finger finger) const;
  bool isNotDown(Leap::Finger finger) const;
};