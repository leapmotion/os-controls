#pragma once
#include "graphics/RenderEngineNode.h"

#include "Primitives.h"

#include "Resource.h"
#include "GLShader.h"

class VolumeControl :
  public RenderEngineNode
{
public:
  VolumeControl();
  ~VolumeControl();

  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;

 EIGEN_MAKE_ALIGNED_OPERATOR_NEW 
private:
  PartialDisk m_partialDisk;
  double m_time;
};
