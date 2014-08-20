#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <map>

#include "CoordinateUtility.h"
#include "HandPointingDecorator.h"

#include <autowiring/Autowired.h>

struct CursorMap : public Eigen::map<int32_t, Vector2> {};

class HandCoordinatesDecorator {
public:
  void AutoFilter(const Leap::Frame& frame, const HandPoseVector& hpm, CursorMap& handScreenLocations);
private:
  AutoRequired<CoordinateUtility> m_coordinateUtility;
};