//
//  CoordianateUtility.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/15/14.
//
//

#include "CoordinateUtility.h"
#include "Autowiring/Autowired.h"
#include <SFML/Window/Window.hpp>

static const Vector3 m_leapMin(-60,200,-30);
static const Vector3 m_leapMax(60,400,30);


static float normalizeFloat(float val, float min, float max) {
  return (val - min) / (max - min);
}

static Vector3 normalizeVec3(Vector3 vec, Vector3 min, Vector3 max) {
  Vector3 retVal;
  retVal.x() = normalizeFloat(vec.x(), min.x(), max.x());
  retVal.y() = normalizeFloat(vec.y(), min.y(), max.y());
  retVal.z() = normalizeFloat(vec.z(), min.z(), max.z());
  return retVal;
}

static Vector2 CoordinateUtility::LeapToScreen(const Leap::Vector leapCoords) {
  Vector2 screenPixels(ScreenNormalToScreenPixels(LeapToScreenNormal(leapCoords)));
  return screenPixels;
}

static Vector3 CoordinateUtility::LeapToScreenNormal(const Leap::Vector leapCoords) {
  const Vector3 eigenLeapCoords = leapCoords.toVector3<Vector3>();
  return normalizeVec3(eigenLeapCoords, m_leapMin, m_leapMax);
}

static Vector2 CoordinateUtility::ScreenNormalToScreenPixels(const Vector3 normalCoords) {
  Autowired<sf::Window> window;
  if(!window)
    throw std::runtime_error("Could not find autowired main window,");
  
  const sf::Vector2u screenSize = window->getSize();
  Vector2 retVal;
  retVal.x() = normalCoords.x() * screenSize.x;
  retVal.y() = normalCoords.y() * screenSize.y;
  
  return retVal;
}