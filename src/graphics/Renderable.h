#pragma once

struct RenderFrame;

class Renderable
{
public:
  /// <summary>
  /// Invoked before any calls to Render to give animation entities opportunity to update their state
  /// </summary>
  virtual void AnimationUpdate(const RenderFrame& frame) = 0;

  /// <summary>
  /// Invoked to perform actual drawing operations on this renderable
  /// </summary>
  virtual void Render(const RenderFrame& frame) const = 0;
};