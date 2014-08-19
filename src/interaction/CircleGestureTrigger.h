//
//  CircleGestureTrigger.h
//  oscontrols
//
//  Created by Paul Mandel on 8/18/14.
//
//

#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include <map>

#include "CoordinateUtility.h"

#include <autowiring/Autowired.h>

struct GestureMap : public std::map<int32_t, Leap::Gesture> {};

class CircleGestureTrigger {
public:
  void AutoFilter(const Leap::Frame& frame, GestureMap& handGestures);
private:
};