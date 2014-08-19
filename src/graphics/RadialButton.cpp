#include "stdafx.h"
#include "RadialButton.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

RadialButton::RadialButton(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset) :
m_innerRadius(innerRadius),
m_outerRadius(innerRadius+width),
m_maxOpacity(0.8)
{
  Translation() = offset;

  m_partialDisk.SetDiffuseColor(Color(0.9f, 0.4f, 0.4f, 0.8f));
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
  m_partialDisk.SetDiffuseColor(Color(0.5f, 0.5f, 0.5f, opacity * m_maxOpacity));
}