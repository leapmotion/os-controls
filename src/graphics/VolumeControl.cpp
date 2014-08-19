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

float VolumeControl::Volume() {
  return volumeFromAngle(m_activePartialDisk.EndAngle());
}

void VolumeControl::SetVolume(float volume) {
  volume = std::max(0.0f, std::min(1.0f, volume));
  double angle = angleFromVolume(volume);
  m_activePartialDisk.SetEndAngle(angle);
}

void VolumeControl::NudgeVolume(float dVolume) {
  std::cout << "dVolume: " << dVolume << std::endl;
  float angle = m_activePartialDisk.EndAngle() + VolumeControl::dAngleFromVolume(dVolume);
  m_activePartialDisk.SetEndAngle(angle);
  std::cout << "New Volume: " << volumeFromAngle(angle) << std::endl;
}

void VolumeControl::AnimationUpdate(const RenderFrame& frame) {
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

float VolumeControl::volumeFromAngle(float angle) {
  return (angle + 5*M_PI/4)/(3*M_PI/2);
}

float VolumeControl::angleFromVolume(float volume) {
  return ((3*M_PI)/2)*volume - (5*M_PI)/4;
}

float VolumeControl::dAngleFromVolume(float dVolume) {
  return ((3*M_PI)/2)*dVolume;
}