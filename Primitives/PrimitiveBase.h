#pragma once

#include "GLMaterial.h"
#include "GLShader.h"
#include "GLShaderBindingScopeGuard.h"
#include "GLShaderLoader.h"
#include "GLShaderMatrices.h"
#include "RenderState.h"
#include "Resource.h"
#include "SceneGraphNode.h"
#include "SceneGraphNodeValues.h"

// This is the base class for drawable, geometric primitives.  It inherits SceneGraphNode<...>
// which provides the "scene graph" design pattern (see Wikipedia article on scene graph),
// as well as some convenience methods.  The template parameter DIM allows 2D or 3D (or even
// 1D) primitives to be defined.
template <int DIM>
class Primitive : public SceneGraphNode<ParticularSceneGraphNodeProperties<MATH_TYPE,DIM,float>> {
public:

  static void DrawSceneGraph(const Primitive &root, RenderState &render_state) {
    // TODO: the existing model view matrix can be inputted as the initial state of global_properties
    // in the call to DepthFirstTraverse.
    root.DepthFirstTraverse<Primitive>([&render_state](const Primitive &node, const Properties &global_properties) {
      node.Draw(render_state, global_properties);
    });
  }

  typedef ParticularSceneGraphNodeProperties<MATH_TYPE,DIM,float> Properties;
  typedef SceneGraphNode<ParticularSceneGraphNodeProperties<MATH_TYPE,DIM,float>> Parent_SceneGraphNode;
  typedef Properties::AffineTransformValue_::Transform Transform;

  Primitive() : m_shader("material") {
    GLMaterial::CheckShaderForUniforms(*m_shader);
  }
  virtual ~Primitive() { }

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

  void Draw(RenderState &render_state, const Properties &global_properties) const {
    // Set the model view (TODO: change this to not be in the RenderState, since it's tracked by DepthFirstTraverse)
    ModelView& model_view = render_state.GetModelView();
    // TODO: make a ScopeGuard for model view matrix.
    model_view.Push();
    model_view.Multiply(SquareMatrixAdaptToDim<4>(global_properties.AffineTransform().AsFullMatrix(), MATH_TYPE(1)));
    MakeAdditionalModelViewTransformations(model_view);

    const GLShader &shader = Shader();
    GLShaderBindingScopeGuard bso(shader, BindFlags::BIND_AND_UNBIND); // binds shader now, unbinds upon end of scope.
    
    GLShaderMatrices::UploadUniforms(shader, model_view.Matrix(), render_state.GetProjection().Matrix(), BindFlags::NONE);
    Material().UploadUniforms(shader, global_properties.AlphaMask(), BindFlags::NONE);

    DrawContents(render_state);

    model_view.Pop(); // TODO: once the ScopeGuard for the model view matrix is created, this goes away.
  }

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

typedef Primitive<3> Primitive3;
// TODO: Deprecate the identifier "PrimitiveBase" and use Primitive3 instead.
typedef Primitive3 PrimitiveBase;
