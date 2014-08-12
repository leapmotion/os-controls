#include "stdafx.h"
#include "VolumeControl.h"
#include "GLShaderLoader.h"


VolumeControl::VolumeControl()
{
  m_shader = Resource<GLShader>("default")

  m_disk.SetInnerRadius(4);
  m_disk.SetOuterRadius(6);
  m_disk.Translation() = 100 * Vector3::UnitX() + 100 * Vector3::UnitY();
  m_disk.SetDiffuseColor(Color(0.7f, 1.0f, 0.4f));
  m_disk.SetAmbientFactor(0.3f);
}

VolumeControl::~VolumeControl()
{
}

void VolumeControl::AutoFilter(const RenderFrame& frame, const StateSentry<State::VolumeControl>& in) {
  const auto size = frame.renderWindow->getSize();
  m_time += frame.deltaT.count();
  
  m_disk.SetStartAngle(m_time / 4.0*M_PI));
  m_disk.SetEndAngle(m_time / 3.0*M_PI));
  
  m_disk
}