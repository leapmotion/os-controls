//
//  CoordianateUtility.cpp
//  Shortcuts
//
//  Created by Daniel Plemmons on 8/15/14.
//
//

#include "CoordinateUtility.h"
#include "InteractionConfigs.h"

float normalizeFloat(float val, float min, float max) {
  return (val - min) / (max - min);
}

EigenTypes::Vector3 normalizeVec3(const EigenTypes::Vector3& vec, const EigenTypes::Vector3& min, const EigenTypes::Vector3& max) {
  EigenTypes::Vector3 retVal;
  retVal.x() = normalizeFloat((float)vec.x(), (float)min.x(), (float)max.x());
  retVal.y() = normalizeFloat((float)vec.y(), (float)min.y(), (float)max.y());
  retVal.z() = normalizeFloat((float)vec.z(), (float)min.z(), (float)max.z());
  return retVal;
}

EigenTypes::Vector2 CoordinateUtility::LeapToScreen(Leap::Vector leapCoords) {
  EigenTypes::Vector2 screenPixels(ScreenNormalToScreenPixels(LeapToScreenNormal(leapCoords)));
  return screenPixels;
}

EigenTypes::Vector3 CoordinateUtility::LeapToScreenNormal(Leap::Vector leapCoords) {
  const EigenTypes::Vector3 eigenLeapCoords = leapCoords.toVector3<EigenTypes::Vector3>();
  return normalizeVec3(eigenLeapCoords, config::m_leapMin, config::m_leapMax);
}

EigenTypes::Vector2 CoordinateUtility::ScreenNormalToScreenPixels(const EigenTypes::Vector3& normalCoords) {
  if(!m_renderWindow)
    return EigenTypes::Vector2(0,0); // If the screen doesn't exist yet, just say we're at (0,0). Ok to fail quiet.

  const auto screenSize = m_renderWindow->GetSize();
  EigenTypes::Vector2 retVal;
  retVal.x() = normalCoords.x() * screenSize.width;
  retVal.y() = normalCoords.y() * screenSize.height;

  return retVal;
}
