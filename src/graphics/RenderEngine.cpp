#include "stdafx.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include "osinterface/OSVirtualScreen.h"
#include "osinterface/MakesRenderWindowFullScreen.h"
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
  m_drewFrame(false)
{
  if (!sf::Shader::isAvailable()) //This also calls glewInit for us
    throw std::runtime_error("Shaders are not supported!");

  m_shader = Resource<GLShader>("material");

  // set light position
  const EigenTypes::Vector3f lightPos(0, 10, 10);
  m_shader->Bind();
  m_shader->SetUniformf("lightPosition", lightPos);
  m_shader->Unbind();
}

RenderEngine::~RenderEngine()
{
}

void RenderEngine::Tick(std::chrono::duration<double> deltaT) {
  // Active the window for OpenGL rendering
  m_rw->setActive();

  // Clear window
  m_rw->clear(sf::Color::Transparent);

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

  bool drewThisFrame = false;

  // Only render objects when the screensaver is disabled
  if (m_virtualScreen && !m_virtualScreen->IsScreenSaverActive()) {
    // Perform render operation in a second pass:
    for(auto& renderable : *this) {
      if (!renderable->IsVisible()) {
        continue;
      }
      drewThisFrame = true;
      auto& mv = frame.renderState.GetModelView();
      mv.Push();

      mv.Translate(EigenTypes::Vector3{renderable->position.x, renderable->position.y, 0.0});
      renderable->Render(frame);
      mv.Pop();
    }
  }

  // General cleanup
  m_shader->Unbind();

  // Update the window
  m_rw->display(); // Always update the display, even if we are just erasing
  m_drewFrame = drewThisFrame;

  m_rw->setActive(false);

  // Show the overlay window if we are rendering, hide it if we aren't
  if (m_makesRenderWindowFullScreen && m_makesRenderWindowFullScreen->IsVisible() != m_drewFrame) {
    m_makesRenderWindowFullScreen->SetVisible(m_drewFrame);
  }
  if (!m_drewFrame) {
    // if we haven't drawn anything, sleep for a bit (otherwise this loop occurs too quickly)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
