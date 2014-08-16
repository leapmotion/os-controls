#pragma once

#include "Leap.h"
#include "EigenTypes.h"

#include "Autowiring/Autowired.h"
#include <SFML/Window/Window.hpp>

class CoordinateUtility {
public:
  Vector2 LeapToScreen(Leap::Vector leapCoords);
  Vector3 LeapToScreenNormal(Leap::Vector leapCoords);
  Vector2 ScreenNormalToScreenPixels(Vector3 normalCoords);
  
private:
  Autowired<sf::Window> m_window;
};