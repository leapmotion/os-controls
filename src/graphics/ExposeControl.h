#pragma once
#include "osinterface/Scene.h"

class ExposeControl
{
public:
  ExposeControl();
  ~ExposeControl();

  void AutoFilter(const Scene& scene);
};

