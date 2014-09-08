//
//  CoordianateUtility.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/15/14.
//
//

#include "CoordinateUtility.h"
#include "InteractionConfigs.h"


float normalizeFloat(float val, float min, float max) {
  return (val - min) / (max - min);
}

Vector3 normalizeVec3(const Vector3& vec, const Vector3& min, const Vector3& max) {
  Vector3 retVal;
  retVal.x() = normalizeFloat((float)vec.x(), (float)min.x(), (float)max.x());
  retVal.y() = normalizeFloat((float)vec.y(), (float)min.y(), (float)max.y());
  retVal.z() = normalizeFloat((float)vec.z(), (float)min.z(), (float)max.z());
  return retVal;
}

Vector2 CoordinateUtility::LeapToScreen(Leap::Vector leapCoords) {
  Vector2 screenPixels(ScreenNormalToScreenPixels(LeapToScreenNormal(leapCoords)));
  return screenPixels;
}

Vector3 CoordinateUtility::LeapToScreenNormal(Leap::Vector leapCoords) {
  const Vector3 eigenLeapCoords = leapCoords.toVector3<Vector3>();
  return normalizeVec3(eigenLeapCoords, config::m_leapMin, config::m_leapMax);
}

Vector2 CoordinateUtility::ScreenNormalToScreenPixels(const Vector3& normalCoords) {
  if(!m_window)
    return Vector2(0,0); // If the screen doesn't exist yet, just say we're at (0,0). Ok to fail quiet.
  
  const sf::Vector2u screenSize = m_window->getSize();
  Vector2 retVal;
  retVal.x() = normalCoords.x() * screenSize.x;
  retVal.y() = normalCoords.y() * screenSize.y;
  
  return retVal;
}