#pragma once
#include "Renderable.h"
#include "SceneGraphNode.h"
#include "uievents/Updatable.h"

class RenderEngineNode :
  public Renderable
{
public:
  RenderEngineNode(void) :
    m_x{0, 0}
  {}

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

  virtual void AnimationUpdate(const RenderFrame& frame) override {};
  virtual void Render(const RenderFrame& frame) const override {};

protected:
  OSVector2 m_x;

public:
  OSVector2& Translation(void) { return m_x; }
  OSVector2 Translation(void) const { return m_x; }
};
