#pragma once

#include "RenderEngineNode.h"
#include <Primitives.h>

class VolumeKnob :
public RenderEngineNode {
public:
  VolumeKnob();
  virtual~VolumeKnob() {};
  
private:
  std::shared_ptr<Disk> knobBody;
  std::shared_ptr<RectanglePrim> knobIndicator;
public:
  
};