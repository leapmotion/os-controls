#pragma once

#include "EigenTypes.h"

/// <summary>
/// Location of a single hand on screen in x-y virtual desktop coordinates
/// </summary>
struct HandLocation {
  float x;
  float y;
  float dX;
  float dY;
  float mmX;
  float mmY;
  float dmmX;
  float dmmY;

  Vector2 screenPosition() const {
    return Vector2(x, y);
  }
  
  Vector2 screenDeltaPos() const {
    return Vector2(dX, dY);
  }
  
  Vector2 leapPosition() const  {
    return Vector2(mmX, mmY);
  }
  
  Vector2 leapDeltaPosition() const {
    return Vector2(dmmX, dmmY);
  }
};