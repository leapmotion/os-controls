#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <map>

#include "CoordinateUtility.h"
#include "HandPointingDecorator.h"

#include <autowiring/Autowired.h>

struct CursorMap : public std::map<int32_t, Vector2> {};

class HandCoordinatesDecorator {
public:
  void AutoFilter(Leap::Frame frame, const HandPointingMap& hpm, CursorMap& handScreenLocations);
private:
  AutoRequired<CoordinateUtility> m_coordinateUtility;
};