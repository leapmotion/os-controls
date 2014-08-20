#pragma once

#include "Material.h"
#include "SceneGraphNode.h"

#include <stack>

class RenderState;

// This is the base class for all 3D primitives.  It inherits SceneGraphNode<...> which
// provides the "scene graph" design pattern (see Wikipedia article on scene graph).
// A primitive can be drawn, and has a diffuse color and an "ambient factor" (TODO:
// what is an ambient factor?).
class PrimitiveBase : public SceneGraphNode<MATH_TYPE,3> {
public:

  typedef SceneGraphNode<MATH_TYPE,3> Parent_SceneGraphNode;
  typedef Parent_SceneGraphNode::Transform Transform;
  typedef std::stack<Transform, std::vector<Transform,Eigen::aligned_allocator<Transform>>> TransformStack;

  PrimitiveBase();
  virtual ~PrimitiveBase() { }

  const ::Material &Material () const { return m_material; }
  ::Material &Material () { return m_material; }
//   const Color& DiffuseColor () const { return m_DiffuseColor; }
//   float AmbientFactor () const { return m_AmbientFactor; }
// 
//   void SetDiffuseColor(const Color& color) { m_DiffuseColor = color; }
//   void SetAmbientFactor(float ambient) { m_AmbientFactor = ambient; }

  // TODO: this sort of doesn't need to be a method, and could be global.
  void DrawScene (RenderState &render_state) const;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:

  // This method should be overridden in each subclass to draw the particular geometry that it represents.
  virtual void Draw (RenderState &render_state, TransformStack &transform_stack) const = 0;

//   Color m_DiffuseColor;
//   float m_AmbientFactor;
  ::Material m_material;
};
