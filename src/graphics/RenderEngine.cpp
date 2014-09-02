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

  //Call AnimationUpdate Depth First (pre-visitation order)
  auto &zList = m_renderList;
  m_rootNode->DepthFirstTraverse(
    [&zList, &frame](RenderEngineNode::BaseSceneNode_t& node){
      auto& mv = frame.renderState.GetModelView();
      mv.Push();
      mv.Translate(node.Translation());
      mv.Multiply(Matrix3x3(node.LinearTransformation()));

      Renderable* renderable = dynamic_cast<Renderable*>(&node);
      if (renderable)
        renderable->AnimationUpdate(frame);

      zList.push_back(std::make_pair(&node, mv.Matrix()));
    },
    [&frame](RenderEngineNode::BaseSceneNode_t& node) {
      frame.renderState.GetModelView().Pop();
    }
  );
    
  //Greatest Z-Values (furthest away) should be rendered first
  std::stable_sort(zList.begin(), zList.end(), 
    [](const RenderListElement_t& a, const RenderListElement_t& b){ return a.first->Translation().z() > b.first->Translation().z(); }
  );

  for (auto &element : zList) {
    frame.renderState.GetModelView().Push();
    frame.renderState.GetModelView().Multiply(element.second);

    Renderable* renderable = dynamic_cast<Renderable*>(element.first);
    if (renderable)
      renderable->Render(frame);
    else{
      PrimitiveBase* primitive = dynamic_cast<PrimitiveBase*>(element.first);
      if (primitive)
        primitive->Draw(frame.renderState);
    }
    frame.renderState.GetModelView().Pop();
  }

  m_renderList.clear(); //Todo: temporal coherency - scan the list to look for changes instead of clearing/rebuilding?

  m_shader->Unbind();

  // Update the window
  m_rw->display();
  m_rw->setActive(false);
}
