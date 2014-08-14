#pragma once

#include "RenderFrame.h"

class Renderable
{
public:
  virtual void AnimationUpdate(const RenderFrame& frame) = 0;
  virtual void Render(const RenderFrame& frame) const = 0;
};