#pragma once

#include "Animation.h"
#include "uiEvents/Updatable.h"
#include <Primitives.h>

class VolumeKnob :
public Updatable,
public PrimitiveBase {
public:
  VolumeKnob();
  ~VolumeKnob();
  
private:
  Disk m_knobBody;
  RectanglePrim m_knobIndicator;
  Smoothed<float> m_opacity;
  
public:
  void Tick(std::chrono::duration<double> deltaT) override;
  void Draw(RenderState &render_state) const override;
  void SetOpacity(float opacity);
};