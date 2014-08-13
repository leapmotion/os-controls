#include "stdafx.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include <GL/glew.h>
#include "GLShader.h"
#include "GLShaderLoader.h"

#include "Resource.h"

#include <SFML/Graphics/Shader.hpp>

RenderEngine::RenderEngine()
{
  m_renderState.GetProjection().Orthographic(0, 800, 600, 0, 0, 100);
 
  if (!sf::Shader::isAvailable()) //This also calls glewInit for us
    throw std::exception("Shaders are not supported!");

  m_shader = Resource<GLShader>("default");

  // set light position
  const Vector3f lightPos(0, 10, 10);
  m_shader->SetUniformf("lightPosition", lightPos);

  m_renderState.SetPositionAttribute(m_shader->LocationOfAttribute("position"));
  m_renderState.SetNormalAttribute(m_shader->LocationOfAttribute("normal"));
  m_renderState.SetModelViewMatrixUniform(m_shader->LocationOfUniform("modelView"));
  m_renderState.SetProjectionMatrixUniform(m_shader->LocationOfUniform("projection"));
  m_renderState.SetNormalMatrixUniform(m_shader->LocationOfUniform("normalMatrix"));
  m_renderState.SetDiffuseColorUniform(m_shader->LocationOfUniform("diffuseColor"));
  m_renderState.SetAmbientFactorUniform(m_shader->LocationOfUniform("ambientFactor"));
}


RenderEngine::~RenderEngine()
{
}

void RenderEngine::Frame(const std::shared_ptr<sf::RenderWindow> &target, const std::chrono::duration<double> deltaT){
  // Active the window for OpenGL rendering
  target->setActive();
  // Clear window
  target->clear(sf::Color::Transparent);
  
  // Pilot a packet through the system:
  auto packet = m_factory->NewPacket();

  // Have objects rendering into the specified window with the supplied change in time
  RenderFrame render = { target, deltaT };

  // Draw all of the objects
  if (packet->HasSubscribers(typeid(RenderFrame))) {
    packet->DecorateImmediate(render);
  }
  // Update the window
  target->display();

  target->setActive(false);
}