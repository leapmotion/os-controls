#include "stdafx.h"
#include "RadialButton.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

//TODO: Update the "offset" concept to work with non-cardinal direcitons
RadialButton::RadialButton(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge) :
m_isNullWedge(isNullWedge),
m_innerRadius(innerRadius),
m_outerRadius(innerRadius+width),
m_maxOpacity(0.8)
{
  Translation() = offset;
  
  m_color_r = 0.7f;
  m_color_g = 0.7f;
  m_color_b = 0.7f;
  m_color_a = m_isNullWedge ? 0.0f : 1.0f;
  
  m_partialDisk.SetDiffuseColor(Color(m_color_r, m_color_g, m_color_b, m_color_a));
  m_partialDisk.SetAmbientFactor(0.9f);
  
  m_partialDisk.SetInnerRadius(m_innerRadius);
  m_partialDisk.SetOuterRadius(m_outerRadius);
  m_partialDisk.SetStartAngle(startAngle);
  m_partialDisk.SetEndAngle(endAngle);
}

RadialButton::~RadialButton() {
}

void RadialButton::Nudge(float offset) {
  m_partialDisk.SetInnerRadius(m_innerRadius + offset);
  m_partialDisk.SetOuterRadius(m_outerRadius + offset);
}

void RadialButton::Render(const RenderFrame& frame) const {
  if (m_partialDisk.DiffuseColor().A() == 0.0f)
    return;

  // draw primitives
  m_partialDisk.Draw(frame.renderState);
}

void RadialButton::SetOpacity(float opacity) {
  if( !m_isNullWedge ) {
    opacity = std::max(0.0f, std::min(m_maxOpacity, opacity));
    m_partialDisk.SetDiffuseColor(Color(0.5f, 0.5f, 0.5f, opacity));
  }
}

//Returns the center of mass in screen coordinates
Vector2 RadialButton::GetCenterOfMass() const {
  float centerAngle = m_partialDisk.StartAngle() + ((m_partialDisk.EndAngle() - m_partialDisk.StartAngle()) / 2.0f);
  float centerDistance = m_innerRadius + ((m_outerRadius - m_innerRadius) / 2.0);
  
  float normalizedX = cosf(centerAngle);
  float normalizedY = sinf(centerAngle);
  
  Vector2 position = ProjectVector( 2, ComputeTransformToGlobalCoordinates().translation() );
  position.x() = position.x() + normalizedX * centerDistance;
  position.y() = position.y() + normalizedY * centerDistance;
  return position;
}

