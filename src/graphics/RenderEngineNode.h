#pragma once

#include "Renderable.h"
#include "Updatable.h"
#include "SceneGraphNode.h"

class RenderEngineNode :
  public SceneGraphNode<double, 3>,
  public Renderable,
  public Updatable
{
public:

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
  virtual void InitChildren() {};

  virtual void Update(double deltaT) override {};
  virtual void AnimationUpdate(const RenderFrame& frame) override {};
  virtual void Render(const RenderFrame& frame) const override {};

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};