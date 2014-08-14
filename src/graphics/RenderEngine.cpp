#include "stdafx.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include <GL/glew.h>
#include "GLShader.h"
#include "GLShaderLoader.h"

#include "Resource.h"

#include <SFML/Graphics/Shader.hpp>
#include <vector>
#include <memory>
#include <algorithm>

RenderEngine::RenderEngine()
{
  m_renderState.GetModelView().Reset();
  m_renderState.GetModelView().LookAt(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3::UnitY());
 
  if (!sf::Shader::isAvailable()) //This also calls glewInit for us
    throw std::runtime_error("Shaders are not supported!");

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

void RenderEngine::Update(const std::chrono::duration<double> deltaT) {
  m_rootNode->DepthFirstTraverse([deltaT](SceneGraphNode<double, 3>& node) {
    RenderEngineNode &renderNode = static_cast<RenderEngineNode &>(node);
    renderNode.Update(deltaT.count());
  }, nullptr);
}

void RenderEngine::Render(const std::shared_ptr<sf::RenderWindow> &target, const std::chrono::duration<double> deltaT){
  // Active the window for OpenGL rendering
  target->setActive();
  // Clear window
  target->clear(sf::Color::Transparent);

  const auto windowSize = target->getSize();
  m_renderState.GetProjection().Orthographic(0, windowSize.y, windowSize.x, 0, 0, 100);
  m_renderState.GetModelView().Reset();

  m_shader->Bind();

  // Have objects rendering into the specified window with the supplied change in time
  RenderFrame frame = { target, m_renderState, deltaT };

  //Call AnimationUpdate Depth First (pre-visitation order)
  auto &zList = m_renderList;
  m_rootNode->DepthFirstTraverse([&zList, &frame](SceneGraphNode<double, 3>& node){
    RenderEngineNode &renderNode = static_cast<RenderEngineNode &>(node);

    auto& mv = frame.renderState.GetModelView();
    mv.Push();
    mv.Translate(node.Translation());
    mv.Multiply(Matrix3x3(node.LinearTransformation()));

    renderNode.AnimationUpdate(frame);
    zList.push_back(std::make_pair(&renderNode, mv));
    },
    [&frame](SceneGraphNode<double, 3>& node) {
      frame.renderState.GetModelView().Pop();
    });

  std::stable_sort(zList.begin(), zList.end(), 
    [](const RenderListElement_t& a, const RenderListElement_t& b){ return a.first->Translation().z() < a.first->Translation().z(); }
  );

  for (auto element : zList) {
    frame.renderState.GetModelView() = element.second;
    element.first->Render(frame);
  }
  m_renderList.clear(); //Todo: temporal coherency - scan the list to look for changes instead of clearing/rebuilding?

  m_shader->Unbind();

  // Update the window
  target->display();

  target->setActive(false);
}
