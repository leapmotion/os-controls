#include "stdafx.h"
#include "VolumeControl.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

VolumeControl::VolumeControl(float radius, float width) :
  m_partialDisk(new PartialDisk()),
  m_activePartialDisk(new PartialDisk()),
  m_needle(new RectanglePrim()),
  m_maxOpacity(0.8f)
{
  m_partialDisk->SetDiffuseColor(Color(0.7f, 0.7f, 0.7f,m_maxOpacity));
  m_partialDisk->SetAmbientFactor(0.9f);
  
  m_partialDisk->SetInnerRadius(radius);
  m_partialDisk->SetOuterRadius(radius + width);
  m_partialDisk->SetStartAngle(-5*M_PI/4);
  m_partialDisk->SetEndAngle(M_PI/4);
  
  
  m_activePartialDisk->SetDiffuseColor(Color(0.3486f, 0.573f, 0.0784f,m_maxOpacity));
  m_activePartialDisk->SetAmbientFactor(0.9f);
  
  m_activePartialDisk->SetInnerRadius(radius);
  m_activePartialDisk->SetOuterRadius(radius + width);
  m_activePartialDisk->SetStartAngle(-5*M_PI/4);
  m_activePartialDisk->SetEndAngle(-5*M_PI/4);
  
  m_needle->SetDiffuseColor(Color(0.7f, 0.7f, 0.7f, m_maxOpacity));
  m_needle->SetAmbientFactor(0.9f);
  
  m_needle->SetSize(Vector2(radius, 2.0f));
  m_needle->FullTransform().translate(Vector3(radius, 0, 0));
  m_needle->FullTransform().rotate(Eigen::AngleAxis<double>(-M_PI/2.0f, Vector3::UnitZ()));
  //  m_needle.Translation() = Vector3(radius, 0, 0);
}

void VolumeControl::InitChildren() {
  AddChild(m_partialDisk);
  AddChild(m_activePartialDisk);
  AddChild(m_needle);
}

float VolumeControl::Volume() {
  return volumeFromAngle(m_activePartialDisk->EndAngle());
}

void VolumeControl::SetVolume(float volume) {
  volume = std::max(0.0f, std::min(1.0f, volume));
  double angle = angleFromVolume(volume);
  m_activePartialDisk->SetEndAngle(angle);
}

void VolumeControl::NudgeVolume(float dVolume) {
  std::cout << "dVolume: " << dVolume << std::endl;
  float angle = m_activePartialDisk->EndAngle() + VolumeControl::dAngleFromVolume(dVolume);
  m_activePartialDisk->SetEndAngle(angle);
  std::cout << "New Volume: " << volumeFromAngle(angle) << std::endl;
}

void VolumeControl::SetOpacity(float opacity) {
  Color c = m_partialDisk->DiffuseColor();
  c.A() = opacity * m_maxOpacity;
  m_partialDisk->SetDiffuseColor(c);
  
  c = m_activePartialDisk->DiffuseColor();
  c.A() = opacity * m_maxOpacity;
  m_activePartialDisk->SetDiffuseColor(c);
  
  c = m_needle->DiffuseColor();
  c.A() = opacity * m_maxOpacity;
  m_needle->SetDiffuseColor(c);
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