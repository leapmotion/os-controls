#pragma once

enum class State {
  VolumeControl,
  ExposeMaker,
  FunnyThirdThing
};

template<State>
class StateSentry {};