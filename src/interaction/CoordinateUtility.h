#pragma once

#include "Leap.h"
#include "EigenTypes.h"

#include "Autowiring/Autowired.h"
#include <SFML/Graphics/RenderWindow.hpp>

class CoordinateUtility {
public:
  Vector2 LeapToScreen(Leap::Vector leapCoords);
  Vector3 LeapToScreenNormal(Leap::Vector leapCoords);
  Vector2 ScreenNormalToScreenPixels(const Vector3& normalCoords);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  Autowired<sf::RenderWindow> m_window;
};