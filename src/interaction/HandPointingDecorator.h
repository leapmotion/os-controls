#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

struct HandPointingMap : public std::map<int32_t, Leap::Hand> {};

class HandPointingDecorator {
public:
  void AutoFilter(Leap::Frame frame, HandPointingMap& pointingHandsMap);
private:
  bool isPointing(Leap::Hand hand) const;
};