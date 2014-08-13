#pragma once

enum class State {
  MediaView,
  ExposeMaker,
  FunnyThirdThing
};

template<State>
class StateSentry {};