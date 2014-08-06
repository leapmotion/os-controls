#pragma once
#include "State.h"
#include "osinterface/Scene.h"

class VolumeControl
{
public:
  VolumeControl();
  ~VolumeControl();

  void AutoFilter(const Scene& scene, const StateSentry<State::VolumeControl>& in);
};
