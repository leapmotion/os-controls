#pragma once
#include "State.h"
#include "graphics/Scene.h"

class VolumeControl
{
public:
  VolumeControl();
  ~VolumeControl();

  void AutoFilter(const Scene& scene, const StateSentry<State::VolumeControl>& in);
};
