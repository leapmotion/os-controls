#pragma once

#include "Leap.h"
#include "EigenTypes.h"

namespace CoordinateUtility {
  static Vector2 LeapToScreen(const Leap::Vector leapCoords);
  static Vector3 LeapToScreenNormal(const Leap::Vector leapCoords);
  static Vector2 ScreenNormalToScreenPixels(const Vector3 normalCoords);
};