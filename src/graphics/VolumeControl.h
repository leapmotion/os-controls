#pragma once
#include "graphics/RenderEngineNode.h"

#include "Primitives.h"

#include "Resource.h"
#include "GLShader.h"

class VolumeControl :
  public RenderEngineNode
{
public:
  VolumeControl(float radius, float width);
  
  void SetVolume(float volume);

  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;
  
  void SetOpacity(float opacity);

 EIGEN_MAKE_ALIGNED_OPERATOR_NEW 
private:
  PartialDisk m_partialDisk;
  PartialDisk m_activePartialDisk;
  double m_time;
};
