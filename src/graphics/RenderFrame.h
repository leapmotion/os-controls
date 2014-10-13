#pragma once

#include <memory>
#include <chrono>

class RenderState;
class RenderWindow;

// Attach to a packet to request a frame to get rendered
struct RenderFrame
{
  std::shared_ptr<RenderWindow> renderWindow;
  RenderState& renderState;
  std::chrono::duration<double> deltaT;
};
