#pragma once

#include "Primitives.h"
#include "SVGPrimitive.h"
#include "Color.h"
#include "Animation.h"

class RadialItem : public PrimitiveBase {
public:

  RadialItem();
  void SetThickness(double thickness) { m_Thickness = thickness; }
  void SetRadius(double radius) { m_Radius = radius; }
  void SetStartAngle(double startAngle) { m_StartAngle = startAngle; }
  void SetEndAngle(double endAngle) { m_EndAngle = endAngle; }

  double Thickness() const { return m_Thickness; }
  double Radius() const { return m_Radius; }
  double StartAngle() const { return m_StartAngle; }
  double EndAngle() const { return m_EndAngle; }

protected:

  Vector2 toRadialCoordinates(const Vector2& pos) const;

  double m_Thickness;
  double m_Radius;
  double m_StartAngle;
  double m_EndAngle;
};
