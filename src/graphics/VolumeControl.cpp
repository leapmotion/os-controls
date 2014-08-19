#include "stdafx.h"
#include "VolumeControl.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

VolumeControl::VolumeControl(float radius, float width) :
m_time(0),
m_maxOpacity(0.8f)
{
  m_partialDisk.SetDiffuseColor(Color(0.7f, 0.7f, 0.7f,m_maxOpacity));
  m_partialDisk.SetAmbientFactor(0.9f);
  
  m_partialDisk.SetInnerRadius(radius);
  m_partialDisk.SetOuterRadius(radius + width);
  m_partialDisk.SetStartAngle(-5*M_PI/4);
  m_partialDisk.SetEndAngle(M_PI/4);
  
  
  m_activePartialDisk.SetDiffuseColor(Color(0.3486f, 0.573f, 0.0784f,m_maxOpacity));
  m_activePartialDisk.SetAmbientFactor(0.9f);
  
  m_activePartialDisk.SetInnerRadius(radius);
  m_activePartialDisk.SetOuterRadius(radius + width);
  m_activePartialDisk.SetStartAngle(-5*M_PI/4);
  m_activePartialDisk.SetEndAngle(-5*M_PI/4);
}

void VolumeControl::SetVolume(float volume) {
  float angle = 3*M_PI/2*volume - 5*M_PI/4;
  m_activePartialDisk.SetEndAngle(angle);
}

void VolumeControl::AnimationUpdate(const RenderFrame& frame) {
  m_time += frame.deltaT.count();

  SetVolume(0.5 + 0.5*std::sin(0.5*m_time));
}

void VolumeControl::Render(const RenderFrame& frame) const {
  if (m_partialDisk.DiffuseColor().A() == 0.0f)
    return;
  
  // draw primitives - they are not *technically part of our graph
  m_partialDisk.Draw(frame.renderState);
  m_activePartialDisk.Draw(frame.renderState);
}

void VolumeControl::SetOpacity(float opacity) {
  Color c = m_partialDisk.DiffuseColor();
  c.A() = opacity * m_maxOpacity;
  m_partialDisk.SetDiffuseColor(c);
  
  c = m_activePartialDisk.DiffuseColor();
  c.A() = opacity * m_maxOpacity;
  m_activePartialDisk.SetDiffuseColor(c);
}