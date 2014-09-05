#pragma once

#include "GLMaterial.h"
#include "GLShaderMatrices.h"
#include "Resource.h"
#include "SceneGraphNode.h"
#include "SceneGraphNodeValues.h"

#include <stack>

class RenderState;

// This is the base class for all 3D primitives.  It inherits SceneGraphNode<...> which
// provides the "scene graph" design pattern (see Wikipedia article on scene graph).
// A primitive can be drawn, and has a diffuse color and an "ambient factor" (TODO:
// what is an ambient factor?).
class PrimitiveBase : public SceneGraphNode<ParticularSceneGraphNodeProperties<MATH_TYPE,3,float>> {
public:
  static void DrawSceneGraph(const PrimitiveBase &root, RenderState &render_state);

public:

  typedef ParticularSceneGraphNodeProperties<MATH_TYPE,3,float> Properties;
  typedef SceneGraphNode<ParticularSceneGraphNodeProperties<MATH_TYPE,3,float>> Parent_SceneGraphNode;
  typedef Properties::AffineTransformValue_::Transform Transform;

  PrimitiveBase();
  virtual ~PrimitiveBase() { }

  const GLMaterial &Material () const { return m_material; }
  GLMaterial &Material () { return m_material; }

  typename Transform::ConstTranslationPart Translation () const { return LocalProperties().AffineTransform().translation(); }
  typename Transform::TranslationPart Translation () { return LocalProperties().AffineTransform().translation(); }
  typename Transform::ConstLinearPart LinearTransformation () const { return LocalProperties().AffineTransform().linear(); }
  typename Transform::LinearPart LinearTransformation () { return LocalProperties().AffineTransform().linear(); }

  // This method should be overridden in each subclass to draw the particular geometry that it represents.
  virtual void Draw(RenderState &render_state) const = 0;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
protected:

  Resource<GLShader> m_shader;
  GLMaterial m_material;
};
