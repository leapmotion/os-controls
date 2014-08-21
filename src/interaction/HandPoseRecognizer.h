#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

enum class HandPose {
  ZeroFingers,
  OneFinger,
  TwoFingers,
  ThreeFingers
};

class HandPoseRecognizer {
public:
  void AutoFilter(const Leap::Hand& frame, HandPose& handPose);
private:
  bool isPointing(Leap::Hand hand, int nFingers) const;
};