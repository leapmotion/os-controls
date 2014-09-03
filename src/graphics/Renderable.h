#pragma once
#include "Vector.h"

struct RenderFrame;

class Renderable
{
public:
  /// <returns>
  /// The position of this renderable entity
  /// </reutrns>
  virtual OSVector2 Translation(void) const = 0;

  /// <summary>
  /// Invoked before any calls to Render to give animation entities opportunity to update their state
  /// </summary>
  virtual void AnimationUpdate(const RenderFrame& frame) = 0;

  /// <summary>
  /// Invoked to perform actual drawing operations on this renderable
  /// </summary>
  virtual void Render(const RenderFrame& frame) const = 0;
};