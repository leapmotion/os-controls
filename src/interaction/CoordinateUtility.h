#pragma once

#include "osinterface/RenderWindow.h"

#include "Leap.h"
#include "EigenTypes.h"

#include "Autowiring/Autowired.h"

class CoordinateUtility {
public:
  EigenTypes::Vector2 LeapToScreen(Leap::Vector leapCoords);
  EigenTypes::Vector3 LeapToScreenNormal(Leap::Vector leapCoords);
  EigenTypes::Vector2 ScreenNormalToScreenPixels(const EigenTypes::Vector3& normalCoords);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Autowired<RenderWindow> m_renderWindow;
};
