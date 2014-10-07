#include "RadialItem.h"

RadialItem::RadialItem() {
  m_StartAngle = 0;
  m_EndAngle = 1;
  m_Radius = 3;
  m_Thickness = 1;
}

EigenTypes::Vector2 RadialItem::toRadialCoordinates(const EigenTypes::Vector2& pos) const {
  const EigenTypes::Vector2 absPos = Translation().head<2>();
  const EigenTypes::Vector2 diff = (pos - absPos);
  double angle = std::atan2(diff.y(), diff.x());
  if (angle < m_StartAngle) {
    angle += 2.0*M_PI;
  }

  return EigenTypes::Vector2(diff.norm(), angle);
}
