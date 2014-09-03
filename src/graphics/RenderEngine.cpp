#include "stdafx.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include <GL/glew.h>
#include "GLShader.h"
#include "GLShaderLoader.h"

#include "Resource.h"
#include "PrimitiveBase.h"

#include <SFML/Graphics/Shader.hpp>
#include <vector>
#include <memory>
#include <algorithm>

RenderEngine::RenderEngine()
{
  if (!sf::Shader::isAvailable()) //This also calls glewInit for us
    throw std::runtime_error("Shaders are not supported!");

  m_shader = Resource<GLShader>("material");

  // set light position
  const Vector3f lightPos(0, 10, 10);
  m_shader->Bind();
  m_shader->SetUniformf("lightPosition", lightPos);
  m_shader->Unbind();
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::BringToFront(Renderable* renderable) {
  auto q = m_renderables.find(renderable);
  if(q == m_renderables.end())
    // Not in the collection, ignore
    return;

  // Move this entry to the front of the list:
  auto i = q->second;
  m_renderList.splice(m_renderList.begin(), m_renderList, i, std::next(i));
}

void RenderEngine::Tick(std::chrono::duration<double> deltaT) {
  // Active the window for OpenGL rendering
  m_rw->setActive();

  // Clear window
  m_rw->clear(sf::Color::Transparent);

  //Set the mode
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const auto windowSize = m_rw->getSize();
  m_renderState.GetProjection().Orthographic(0, windowSize.y, windowSize.x, 0, 1, -100);
  m_renderState.GetModelView().Clear();

  m_shader->Bind();

  // Have objects rendering into the specified window with the supplied change in time
  RenderFrame frame = {m_rw, m_renderState, deltaT};

  // AnimationUpdate all attached nodes
  for(const auto& renderable : m_renderList)
    renderable->AnimationUpdate(frame);
  
  // Perform render operation in a second pass:
  for(auto& renderable : m_renderList) {
    auto& mv = frame.renderState.GetModelView();
    mv.Push();
    auto translation = renderable->Translation();

    mv.Translate(Vector3{translation.x, translation.y, 0.0});
    renderable->Render(frame);
    mv.Pop();
  }

  m_renderList.clear(); //Todo: temporal coherency - scan the list to look for changes instead of clearing/rebuilding?

  m_shader->Unbind();

  // Update the window
  m_rw->display();
  m_rw->setActive(false);
}
