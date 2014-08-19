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

Vector3 normalizeVec3(Vector3 vec, Vector3 min, Vector3 max) {
  Vector3 retVal;
  retVal.x() = normalizeFloat(vec.x(), min.x(), max.x());
  retVal.y() = normalizeFloat(vec.y(), min.y(), max.y());
  retVal.z() = normalizeFloat(vec.z(), min.z(), max.z());
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

Vector2 CoordinateUtility::ScreenNormalToScreenPixels(Vector3 normalCoords) {
  if(!m_window)
    throw std::runtime_error("Could not find autowired main window,");
  
  const sf::Vector2u screenSize = m_window->getSize();
  Vector2 retVal;
  retVal.x() = normalCoords.x() * screenSize.x;
  retVal.y() = normalCoords.y() * screenSize.y;
  
  return retVal;
}