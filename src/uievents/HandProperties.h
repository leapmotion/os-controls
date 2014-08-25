#pragma once

#include "EigenTypes.h"

/// <summary>
/// Location of a single hand on screen in x-y virtual desktop coordinates
/// </summary>
struct HandLocation {
  float x;
  float y;

  Vector2 screenPosition() const {
    return Vector2(x, y);
  }
};