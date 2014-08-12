#pragma once

#include <memory>
#include <chrono>
#include <SFML/Graphics/RenderTarget.hpp>

struct RenderFrame
{
  std::shared_ptr<sf::RenderTarget> renderWindow;
  std::chrono::duration<double> deltaT;
};

