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

  EigenTypes::Vector2 screenPosition() const {
    return EigenTypes::Vector2(x, y);
  }

  EigenTypes::Vector2 screenDeltaPos() const {
    return EigenTypes::Vector2(dX, dY);
  }

  EigenTypes::Vector2 leapPosition() const  {
    return EigenTypes::Vector2(mmX, mmY);
  }

  EigenTypes::Vector2 leapDeltaPosition() const {
    return EigenTypes::Vector2(dmmX, dmmY);
  }
};
