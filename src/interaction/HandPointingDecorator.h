#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

struct HandPointMap : public std::map<int32_t, Leap::Hand> {};
struct HandPoseVector : public std::vector<HandPointMap> {};

enum class HandPoses {
  ZeroFingers,
  OneFinger,
  TwoFingers,
  ThreeFingers
};

class HandPointingDecorator {
public:
  void AutoFilter(const Leap::Hand& frame, HandPoses& handPose);
};