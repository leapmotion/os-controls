#pragma once

#include "EigenTypes.h"

/// <summary>
/// Location of a single hand on screen in x-y virtual desktop coordinates
/// </summary>
struct HandLocation {
  // X position in screen coordinates.
  float x;
  // Y position in screen coordinates.
  float y;
  // Delta X since the last sample in screen coordinates.
  float dX;
  // Delta Y since the last sample in screen coordinates.
  float dY;
  // X position in millimeters.
  float mmX;
  // Y position in millimeters.
  float mmY;
  // Delta X since the last sample in millimeters.
  float dmmX;
  // Delta Y since the last sample in millimeters.
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
