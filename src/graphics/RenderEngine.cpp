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
#include <thread>
#include <chrono>

RenderEngine::RenderEngine() :
  m_drewThisFrame(false),
  m_drewPrevFrame(false)
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

void RenderEngine::Tick(std::chrono::duration<double> deltaT) {
  // Active the window for OpenGL rendering
  InvalidateRect(NULL, NULL, TRUE);
  m_rw->setActive();
  
  // Clear window
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);

  glClearColor(0, .1, .9, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  unsigned char bytes[10];
  glReadPixels(1, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &bytes);
  /*
  //Set the mode
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);

  const auto windowSize = m_rw->getSize();
  m_renderState.GetProjection().Orthographic(0, windowSize.y, windowSize.x, 0, 1, -100);
  m_renderState.GetModelView().Clear();

  m_shader->Bind();

  // Have objects rendering into the specified window with the supplied change in time
  RenderFrame frame = {m_rw, m_renderState, deltaT};

  // AnimationUpdate all attached nodes
  for(const auto& renderable : *this)
    renderable->AnimationUpdate(frame);
  
  m_drewThisFrame = false;

  // Perform render operation in a second pass:
  for(auto& renderable : *this) {
    if (!renderable->IsVisible()) {
      continue;
    }
    m_drewThisFrame = true;
    auto& mv = frame.renderState.GetModelView();
    mv.Push();
    
    mv.Translate(Vector3{renderable->position.x, renderable->position.y, 0.0});
    renderable->Render(frame);
    mv.Pop();
  }

  // General cleanup
  m_shader->Unbind();
  */
  if (m_drewThisFrame || m_drewPrevFrame) {
    // Update the window
    m_rw->display();
  } else {
    // if we haven't drawn anything, sleep for a bit (otherwise this loop occurs too quickly)
    std::chrono::milliseconds delay(100); // 100 msec
    std::this_thread::sleep_for(delay);
  }
  m_drewPrevFrame = m_drewThisFrame;

  m_rw->setActive(false);

  
}
