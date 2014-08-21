#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <map>

#include "CoordinateUtility.h"
#include "HandPoseRecognizer.h"

#include <autowiring/Autowired.h>

struct HandLocation {
  Vector2 screenCoordinates;
};

class HandLocationRecognizer {
public:
  void AutoFilter(const Leap::Hand& hand, const HandPose& handPose, HandLocation& handLocation);
private:
  AutoRequired<CoordinateUtility> m_coordinateUtility;
};