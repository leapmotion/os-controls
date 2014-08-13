#include "stdafx.h"
#include "VolumeControl.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

VolumeControl::VolumeControl() :
m_time(0)
{
  m_partialDisk.Translation() = Vector3(600, 600, 0);
  m_partialDisk.SetDiffuseColor(Color(1.0f, 0.3f, 1.0f, 1.0f));
  m_partialDisk.SetAmbientFactor(0.9f);
}

VolumeControl::~VolumeControl()
{
}

void VolumeControl::draw(const RenderFrame& frame) {
  const auto size = frame.renderWindow->getSize();
  
  m_time += frame.deltaT.count(); 

  m_partialDisk.SetStartAngle(0);
  m_partialDisk.SetEndAngle(2 * M_PI * (0.5 + 0.5*std::sin(0.5*m_time)));
  m_partialDisk.SetInnerRadius(50 + 5*std::cos(m_time));
  m_partialDisk.SetOuterRadius(100 + 8*std::cos(0.66*m_time));

  // draw primitives
  m_partialDisk.DrawScene(*frame.renderState);
}