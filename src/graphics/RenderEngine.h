#pragma once
#include "RenderEngineNode.h"

//Components
#include "RenderState.h"

#include <autowiring/Autowired.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/GlResource.hpp>
#include <chrono>
#include <vector>

class GLShader;

class RenderEngine :
  public sf::GlResource
{
public:
  RenderEngine();
  ~RenderEngine();

  template<typename T>
  void AddSceneNode(std::shared_ptr<T> &node){
    m_rootNode->AddChild(node);
  }

  void Render(const std::shared_ptr<sf::RenderWindow> &target, const std::chrono::duration<double> deltaT);

  //This should probably not be controlled by the RenderEngine
  void Update(const std::chrono::duration<double> deltaT);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:

  Autowired<RenderEngineNode> m_rootNode;

  RenderState m_renderState;
  std::shared_ptr<GLShader> m_shader;

  typedef std::pair<RenderEngineNode::BaseSceneNode_t*, Matrix4x4> RenderListElement_t;
  mutable Eigen::vector<RenderListElement_t> m_renderList;
};

