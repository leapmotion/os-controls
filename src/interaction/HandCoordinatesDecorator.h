#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <map>

#include <autowiring/Autowired.h>

class HandCoordinatesDecorator {
public:
  typedef std::map<int32_t, Vector2> CursorMap;
  void AutoFilter(Leap::Frame frame, CursorMap& handScreenLocations);
};