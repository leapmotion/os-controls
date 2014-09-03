#pragma once

#include "RenderEngineNode.h"
#include "Animation.h"
#include <Primitives.h>

class VolumeKnob :
public RenderEngineNode {
public:
  VolumeKnob();
  ~VolumeKnob();
  
private:
  Disk m_knobBody;
  RectanglePrim m_knobIndicator;
  Smoothed<float> m_opacity;
  
public:
  void AnimationUpdate(const RenderFrame& renderFrame) override;
  void Render(const RenderFrame& renderFrame) const override;
  void SetOpacity(float opacity);
};