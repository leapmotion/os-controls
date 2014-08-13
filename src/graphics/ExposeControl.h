#pragma once

#include "RenderFrame.h"
#include "State.h"

class ExposeControl
{
public:
  ExposeControl();
  ~ExposeControl();

  void AutoFilter(const RenderFrame& frame, const StateSentry<State::ExposeMaker>& in);
};

