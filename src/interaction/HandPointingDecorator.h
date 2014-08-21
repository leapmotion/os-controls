#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

class HandPoseDecorator {
public:
  void AutoFilter(Leap::Hand* pHand);
private:
  bool isPointing(Leap::Hand hand, int nFingers) const;
};