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
  
  void InitChildren() override;

  float Volume();
  void SetVolume(float volume);
  void NudgeVolume(float dVolume);

  void SetAlphaMask(float alphaMask);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  std::shared_ptr<PartialDisk> m_partialDisk;
  std::shared_ptr<PartialDisk> m_activePartialDisk;
  
  float m_maxAlphaMask;
  
  float volumeFromAngle(float angle);
  float angleFromVolume(float volume);
  float dAngleFromVolume(float dVolume);
};
