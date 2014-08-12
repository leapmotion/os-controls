#pragma once
#include "State.h"
#include "graphics/RenderFrame.h"
#include "Primitives.h"

#include "Resource.h"
#include "GLShader.h"

class VolumeControl
{
public:
  VolumeControl();
  ~VolumeControl();

  void AutoFilter(const RenderFrame& frame, const StateSentry<State::VolumeControl>& in);
private:
  PartialDisk m_disk;
  double m_time;

  Resource<GLShader> m_shader;
};
