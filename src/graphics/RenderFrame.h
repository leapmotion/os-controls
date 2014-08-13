#pragma once

#include <memory>
#include <chrono>
#include <SFML/Graphics/RenderTarget.hpp>

class RenderState;

struct RenderFrame
{
  std::shared_ptr<sf::RenderTarget> renderWindow;
  RenderState* renderState;
  std::chrono::duration<double> deltaT;
};

