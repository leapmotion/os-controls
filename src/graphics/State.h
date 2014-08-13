#pragma once

#include "SceneGraphNode.h"
enum class State {
  VolumeControl,
  ExposeMaker,
  FunnyThirdThing
};

template<State>
class StateSentry {};