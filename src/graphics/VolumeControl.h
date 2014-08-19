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
  
  float Volume();
  void SetVolume(float volume);
  void NudgeVolume(float dVolume);

  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;
  
  void SetOpacity(float opacity);

 EIGEN_MAKE_ALIGNED_OPERATOR_NEW 
private:
  PartialDisk m_partialDisk;
  PartialDisk m_activePartialDisk;
  double m_time;
  
  float m_maxOpacity;
  
  float volumeFromAngle(float angle);
  float angleFromVolume(float volume);
  float dAngleFromVolume(float dVolume);
};
