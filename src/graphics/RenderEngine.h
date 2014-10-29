#pragma once
#include "Renderable.h"
#include "uievents/Updatable.h"
#include "osinterface/RenderContext.h"

//Components
#include "RenderState.h"

#include <autowiring/Autowired.h>
#include <chrono>
#include <vector>

namespace Leap {
namespace GL {

class GLShader;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.

class OSVirtualScreen;
class RenderWindow;

//Tracks & dispatches update & render calls to a SceneGraph. Intended for use
//with RenderEngineNodes, but will also support PrimitiveBase children.  Eventually
//destined for the components repo, and when that happens we should unify the type
//hierarchy and remove the need for dynamic casts by either making SceneGraphNode
//Renderable and Updatable, or by making PrimitiveBase a RenderEngineNode.
class RenderEngine :
  public Updatable,
  public Renderable::ZOrderList,
  public RenderContextResource
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
  Autowired<OSVirtualScreen> m_virtualScreen;
  Autowired<RenderWindow> m_renderWindow;

  bool m_drewFrame;
  RenderState m_renderState;
  std::shared_ptr<GLShader> m_shader;
};
