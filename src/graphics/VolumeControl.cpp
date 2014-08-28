#include "stdafx.h"
#include "VolumeControl.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

VolumeControl::VolumeControl(float radius, float width) :
  m_partialDisk(new PartialDisk()),
  m_activePartialDisk(new PartialDisk()),
  m_maxOpacity(0.8f)
{
  m_partialDisk->Material().SetDiffuseLightColor(Color(0.7f, 0.7f, 0.7f,m_maxOpacity));
  m_partialDisk->Material().SetAmbientLightColor(Color(0.7f, 0.7f, 0.7f,m_maxOpacity));
  m_partialDisk->Material().SetAmbientLightingProportion(0.9f);
  
  m_partialDisk->SetInnerRadius(radius);
  m_partialDisk->SetOuterRadius(radius + width);
  m_partialDisk->SetStartAngle(-5*M_PI/4);
  m_partialDisk->SetEndAngle(M_PI/4);
  
  
  m_activePartialDisk->Material().SetDiffuseLightColor(Color(0.3486f, 0.573f, 0.0784f,m_maxOpacity));
  m_activePartialDisk->Material().SetAmbientLightColor(Color(0.3486f, 0.573f, 0.0784f,m_maxOpacity));
  m_activePartialDisk->Material().SetAmbientLightingProportion(0.9f);
  
  m_activePartialDisk->SetInnerRadius(radius);
  m_activePartialDisk->SetOuterRadius(radius + width);
  m_activePartialDisk->SetStartAngle(-5*M_PI/4);
  m_activePartialDisk->SetEndAngle(-5*M_PI/4);
}

void VolumeControl::InitChildren() {
  AddChild(m_partialDisk);
  AddChild(m_activePartialDisk);
}

float VolumeControl::Volume() {
  return volumeFromAngle((float)m_activePartialDisk->EndAngle());
}

void VolumeControl::SetVolume(float volume) {
  volume = std::max(0.0f, std::min(1.0f, volume));
  double angle = angleFromVolume(volume);
  m_activePartialDisk->SetEndAngle(angle);
}

void VolumeControl::NudgeVolume(float dVolume) {
  std::cout << "dVolume: " << dVolume << std::endl;
  float angle = (float)m_activePartialDisk->EndAngle() + VolumeControl::dAngleFromVolume(dVolume);
  m_activePartialDisk->SetEndAngle(angle);
  std::cout << "New Volume: " << volumeFromAngle(angle) << std::endl;
}

void VolumeControl::SetOpacity(float opacity) {
  Color c = m_partialDisk->Material().DiffuseLightColor();
  c.A() = opacity * m_maxOpacity;
  m_partialDisk->Material().SetDiffuseLightColor(c);
  m_partialDisk->Material().SetAmbientLightColor(c);
  
  c = m_activePartialDisk->Material().DiffuseLightColor();
  c.A() = opacity * m_maxOpacity;
  m_activePartialDisk->Material().SetDiffuseLightColor(c);
  m_activePartialDisk->Material().SetAmbientLightColor(c);
}

float VolumeControl::volumeFromAngle(float angle) {
  return (float)((angle + 5*M_PI/4)/(3*M_PI/2));
}

float VolumeControl::angleFromVolume(float volume) {
  return (float)(((3 * M_PI) / 2)*volume - (5 * M_PI) / 4);
}

float VolumeControl::dAngleFromVolume(float dVolume) {
  return (float) (((3 * M_PI) / 2)*dVolume);
}
