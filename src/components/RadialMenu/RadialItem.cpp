#include "RadialItem.h"

RadialItem::RadialItem() {
  m_StartAngle = 0;
  m_EndAngle = 1;
  m_Radius = 3;
  m_Thickness = 1;
}

Vector2 RadialItem::toRadialCoordinates(const Vector2& pos) const {
  const Vector2 absPos = Translation().head<2>();
  const Vector2 diff = (pos - absPos);
  double angle = std::atan2(diff.y(), diff.x());
  if (angle < m_StartAngle) {
    angle += 2.0*M_PI;
  }

  return Vector2(diff.norm(), angle);
}
