#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <map>

#include "CoordinateUtility.h"
#include "HandPointingDecorator.h"

#include <autowiring/Autowired.h>

struct RollMap : public Eigen::map<int32_t, float> {};

class HandRollDecorator {
public:
  void AutoFilter(const Leap::Frame& frame, const HandPoseVector& hpm, RollMap& handRolls);
private:
};