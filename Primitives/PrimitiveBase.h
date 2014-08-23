#pragma once

#include "GLMaterial.h"
#include "GLShaderMatrices.h"
#include "Resource.h"
#include "SceneGraphNode.h"

#include <stack>

class RenderState;

// This is the base class for all 3D primitives.  It inherits SceneGraphNode<...> which
// provides the "scene graph" design pattern (see Wikipedia article on scene graph).
// A primitive can be drawn, and has a diffuse color and an "ambient factor" (TODO:
// what is an ambient factor?).
class PrimitiveBase : public SceneGraphNode<MATH_TYPE,3> {
public:
  static void DrawSceneGraph(const PrimitiveBase &root, RenderState &render_state); 

public:

  typedef SceneGraphNode<MATH_TYPE,3> Parent_SceneGraphNode;
  typedef Parent_SceneGraphNode::Transform Transform;

  PrimitiveBase();
  virtual ~PrimitiveBase() { }

  const GLMaterial &Material () const { return m_material; }
  GLMaterial &Material () { return m_material; }

  // This method should be overridden in each subclass to draw the particular geometry that it represents.
  virtual void Draw(RenderState &render_state) const = 0;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
protected:
  Resource<GLShader> m_shader;
  GLMaterial m_material;
};
