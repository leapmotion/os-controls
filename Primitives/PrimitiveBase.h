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
class PrimitiveBase : public SceneGraphNode<ParticularSceneGraphNodeProperties<EigenTypes::MATH_TYPE,3,float>> {
public:
  static void DrawSceneGraph(const PrimitiveBase &root, RenderState &render_state);

public:

  typedef ParticularSceneGraphNodeProperties<EigenTypes::MATH_TYPE,3,float> Properties;
  typedef SceneGraphNode<ParticularSceneGraphNodeProperties<EigenTypes::MATH_TYPE,3,float>> Parent_SceneGraphNode;
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

  Transform::ConstTranslationPart Translation () const { return LocalProperties().AffineTransform().translation(); }
  Transform::TranslationPart Translation () { return LocalProperties().AffineTransform().translation(); }
  Transform::ConstLinearPart LinearTransformation () const { return LocalProperties().AffineTransform().linear(); }
  Transform::LinearPart LinearTransformation () { return LocalProperties().AffineTransform().linear(); }

  void Draw(RenderState &render_state, const Properties &global_properties) const;

  // This method should be overridden in any subclass that needs to do secondary
  // transformations (e.g. scaling based on a sphere's 'radius' member).
  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const { }

protected:

  // This method should be overridden in each subclass to draw the particular geometry that it represents.
  virtual void DrawContents(RenderState &render_state) const = 0;
  
private:

  Resource<GLShader> m_shader;
  GLMaterial m_material;
};
