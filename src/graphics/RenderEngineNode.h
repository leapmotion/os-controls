#pragma once

#include "Renderable.h"
#include "SceneGraphNode.h"
#include "uievents/Updatable.h"

class RenderEngineNode :
  public SceneGraphNode<double, 3>,
  public Renderable
{
public:
  typedef SceneGraphNode<double, 3> BaseSceneNode_t;

  template<typename T, typename... _Args>
  static std::shared_ptr<T> Create(_Args&&... args) {
    auto ptr = std::shared_ptr<T>(new T(args...)); //cannot use make_shared due to eigen alignment requirements.
    ptr->InitChildren();
    return ptr;
  }

  template<typename T>
  void AddChild(std::shared_ptr<T> &node) {
    std::shared_ptr<SceneGraphNode<double, 3>> sceneNode = std::dynamic_pointer_cast<SceneGraphNode<double, 3>>(node);
    SceneGraphNode<double,3>::AddChild(sceneNode);
  }
  
  //call AddChild for child members here!
  //This is required because to add something to a scene graph, it must have a shared_ptr that
  //owns it, which is impossible in a constructor.
  virtual void InitChildren() {};

  virtual void AnimationUpdate(const RenderFrame& frame) override {};
  virtual void Render(const RenderFrame& frame) const override {};
};