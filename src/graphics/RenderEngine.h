#pragma once
#include "Renderable.h"
#include "uievents/Updatable.h"

//Components
#include "RenderState.h"

#include <autowiring/Autowired.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/GlResource.hpp>
#include <chrono>
#include <vector>

class GLShader;

//Tracks & dispatches update & render calls to a SceneGraph. Intended for use
//with RenderEngineNodes, but will also support PrimitiveBase children.  Eventually
//destined for the components repo, and when that happens we should unify the type
//hierarchy and remove the need for dynamic casts by either making SceneGraphNode
//Renderable and Updatable, or by making PrimitiveBase a RenderEngineNode.
class RenderEngine :
  public Updatable,
  public Renderable::ZOrderList,
  public sf::GlResource
{
public:
  RenderEngine();
  ~RenderEngine();

  /// <summary>
  /// Adds the specified node to the bottom of the render heirarchy
  /// </summary>
  void AddSceneNode(std::shared_ptr<Renderable> &node);

  void Tick(std::chrono::duration<double> deltaT) override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  Autowired<sf::RenderWindow> m_rw;

  bool m_drewThisFrame;
  bool m_drewPrevFrame;
  RenderState m_renderState;
  std::shared_ptr<GLShader> m_shader;
};

