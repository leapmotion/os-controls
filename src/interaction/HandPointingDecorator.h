#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

struct HandPointMap : public std::map<int32_t, Leap::Hand> {};
struct HandPoseVector : public std::vector<HandPointMap> {};

class HandPointingDecorator {
public:
  void AutoFilter(Leap::Frame frame, HandPoseVector& poseVector);
private:
  bool isPointing(Leap::Hand hand, int nFingers) const;
};