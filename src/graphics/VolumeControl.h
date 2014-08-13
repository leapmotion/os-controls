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

  void draw(const RenderFrame& frame);

 EIGEN_MAKE_ALIGNED_OPERATOR_NEW 
private:
  PartialDisk m_partialDisk;
  double m_time;
};
