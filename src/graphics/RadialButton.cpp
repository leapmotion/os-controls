#include "stdafx.h"
#include "RadialButton.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

RadialButton::RadialButton(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset) {
  Translation() = offset;

  m_partialDisk.SetDiffuseColor(Color(0.5f, 0.5f, 0.5f, 0.7f));
  m_partialDisk.SetAmbientFactor(0.9f);
  
  m_innerRadius = innerRadius;
  m_outerRadius = innerRadius + width;
  
  m_partialDisk.SetInnerRadius(m_innerRadius);
  m_partialDisk.SetOuterRadius(m_outerRadius);
  m_partialDisk.SetStartAngle(startAngle);
  m_partialDisk.SetEndAngle(endAngle);
}

RadialButton::~RadialButton() {
}

void RadialButton::nudge(float offset) {
  m_partialDisk.SetInnerRadius(m_innerRadius + offset);
  m_partialDisk.SetOuterRadius(m_outerRadius + offset);
}

void RadialButton::Render(const RenderFrame& frame) const {
  if (m_partialDisk.DiffuseColor().A() == 0.0f)
    return;

  // draw primitives
  m_partialDisk.Draw(frame.renderState);
}

void RadialButton::setOpacity(float opacity) {
  //opacity = std::max(0.0f, std::min(1.0f, opacity));
  m_partialDisk.SetDiffuseColor(Color(0.5f, 0.5f, 0.5f, opacity));
}