#pragma once

#include "GLMaterial.h"
#include "GLShader.h"
#include "GLShaderMatrices.h"
#include "Resource.h"
#include "SceneGraphNode.h"
#include "SceneGraphNodeValues.h"

#include <stack>

class ModelView;
class RenderState;

// This is the base class for all 3D drawable, geometric primitives.  It inherits SceneGraphNode<...>
// which provides the "scene graph" design pattern (see Wikipedia article on scene graph).
class PrimitiveBase : public SceneGraphNode<ParticularSceneGraphNodeProperties<MATH_TYPE,3,float>> {
public:
  static void DrawSceneGraph(const PrimitiveBase &root, RenderState &render_state);

public:

  typedef ParticularSceneGraphNodeProperties<MATH_TYPE,3,float> Properties;
  typedef SceneGraphNode<ParticularSceneGraphNodeProperties<MATH_TYPE,3,float>> Parent_SceneGraphNode;
  typedef Properties::AffineTransformValue_::Transform Transform;

  PrimitiveBase();
  virtual ~PrimitiveBase() { }

  const GLShader &Shader () const {
    if (!m_shader) {
      throw std::runtime_error("shader member was not initialized");
    }
    return *m_shader;
  }
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

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const { }

private:

  Resource<GLShader> m_shader;
  GLMaterial m_material;
};
