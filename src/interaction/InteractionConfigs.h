//
//  InteractionConfigs.h
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
//

#pragma once
#include "EigenTypes.h"

namespace config {
  const Vector3 m_leapMin(-120.0f,300.0f,-30.0f);
  const Vector3 m_leapMax(120.0f,600.0f,30.0f);
  const float MIN_PINCH_FOR_PINCHING = 0.74f;
  const float MAX_PINCH_FOR_NOT_PINCHING = 0.69f;
//  const float MAX_DOT_FOR_THUMB_POINTING = 0.1f;
  const float MIN_DOT_FOR_START_POINTING = 0.8f;
  const float MAX_DOT_FOR_CONTINUE_POINTING = 0.6f;
  const float MAX_DOT_FOR_START_CURLED = 0.4f;
  const float MAX_DOT_FOR_CONTINUE_CURLED = 0.5f;
  const float MIN_DOT_FOR_CLAW = 0.5;
  const float MAX_DOT_FOR_CLAW = 0.95f;
  const float MIN_DISTANCE_FOR_CLAW = 15.0f;
  const float FINGER_OFFSET_DISTANCE = 3.0f;
  const float OFFSET_DISTANCE = 100;
  const float ROLL_SENSITIVITY = 1.2f; // don't drop below 1.0
  const float MAX_PINCH_FOR_MENUS = 0.5f;
}
