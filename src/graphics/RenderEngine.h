#pragma once

//Components
#include "RenderState.h"

#include <autowiring/Autowired.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/GlResource.hpp>
#include <chrono>

class GLShader;

class RenderEngine :
  public sf::GlResource
{
public:
  RenderEngine();
  ~RenderEngine();

  void Frame(const std::shared_ptr<sf::RenderWindow> &target, const std::chrono::duration<double> deltaT);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:

  AutoRequired<AutoPacketFactory> m_factory;

  RenderState m_renderState;
  std::shared_ptr<GLShader> m_shader;
};

