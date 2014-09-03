#pragma once
#include "RenderEngineNode.h"
#include "uievents/Updatable.h"

//Components
#include "RenderState.h"

#include <autowiring/Autowired.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/GlResource.hpp>
#include <chrono>
#include <vector>

class GLShader;

class RootRenderEngineNode : public RenderEngineNode {};

//Tracks & dispatches update & render calls to a SceneGraph. Intended for use
//with RenderEngineNodes, but will also support PrimitiveBase children.  Eventually
//destined for the components repo, and when that happens we should unify the type
//hierarchy and remove the need for dynamic casts by either making SceneGraphNode
//Renderable and Updatable, or by making PrimitiveBase a RenderEngineNode.
class RenderEngine :
  public Updatable,
  public sf::GlResource
{
public:
  RenderEngine();
  ~RenderEngine();

  /// <summary>
  /// Adds the specified node to the bottom of the render heirarchy
  /// </summary>
  void AddSceneNode(std::shared_ptr<Renderable> &node);

  /// <summary>
  /// Moves the specified renderable entity to the front of the render stack
  /// </summary>
  void BringToFront(Renderable* renderable);

  void Tick(std::chrono::duration<double> deltaT) override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  Autowired<sf::RenderWindow> m_rw;

  RenderState m_renderState;
  std::shared_ptr<GLShader> m_shader;

  // Z-order list of elements to be rendered
  typedef std::list<std::shared_ptr<Renderable>> t_renderList;
  t_renderList m_renderList;
  std::unordered_map<Renderable*, t_renderList::iterator> m_renderables;
};

