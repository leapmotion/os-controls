#pragma once

#include "GLShaderLoader.h"
#include "Leap/GL/GLMaterial.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLShaderBindingScopeGuard.h"
#include "Leap/GL/GLShaderMatrices.h"
#include "Leap/GL/SceneGraphNode.h"
#include "Leap/GL/SceneGraphNodeValues.h"
#include "RenderState.h"
#include "Resource.h"

// This is the base class for drawable, geometric primitives.  It inherits SceneGraphNode<...>
// which provides the "scene graph" design pattern (see Wikipedia article on scene graph),
// as well as some convenience methods.  The template parameter DIM allows 2D or 3D (or even
// 1D) primitives to be defined.
template <int DIM>
class Primitive : public SceneGraphNode<ParticularSceneGraphNodeProperties<EigenTypes::MATH_TYPE,DIM,float>> {
public:

  static void DrawSceneGraph(const Primitive &root, RenderState &render_state) {
    // TODO: the existing model view matrix can be inputted as the initial state of global_properties
    // in the call to DepthFirstTraverse.
    root.template DepthFirstTraverse<Primitive>([&render_state](const Primitive &node, const Properties &global_properties) {
      node.Draw(render_state, global_properties);
    });
  }
  // Computes a "squash and stretch" volume-preserving shearing matrix based on a velocity vector
  // The speed denominator controls the shear strength such that a higher value gives less shear
  // When the magnitude of the velocity is equal to speedDenom, the object will be twice as long
  // See http://en.wikipedia.org/wiki/Squash_and_stretch and http://en.wikipedia.org/wiki/Shear_mapping
  static EigenTypes::Matrix3x3 SquashStretchTransform(const EigenTypes::Vector3& velocity, const EigenTypes::Vector3& viewDirection, double speedDenom = 900.0) {
    // compute velocity magnitude and direction
    const double speed = velocity.norm();
    static const double EPSILON = 0.0001;
    if (speedDenom < EPSILON || speed < EPSILON*speedDenom) {
      return EigenTypes::Matrix3x3::Identity();
    }
    const EigenTypes::Vector3 direction = velocity / speed;

    // compute stretch and squash multipliers (volume preserving)
    const double stretch = 1.0 + std::min(1.0, speed / speedDenom);
    const double squash = std::sqrt(1.0 / stretch);

    // compute velocity basis and its inverse for rotation
    EigenTypes::Matrix3x3 velocityBasis(EigenTypes::Matrix3x3::Identity());
    velocityBasis.col(0) = direction;
    velocityBasis.col(1) = direction.cross(viewDirection);
    velocityBasis.col(2) = viewDirection;
    const EigenTypes::Matrix3x3 velocityBasisInv = velocityBasis.inverse();

    // compute scale matrix for deformation
    const EigenTypes::Matrix3x3 scaleMatrix = EigenTypes::Vector3(stretch, squash, squash).asDiagonal();

    // undo rotation, deform, then rotate back
    return velocityBasis * scaleMatrix * velocityBasisInv;
  }

  typedef ParticularSceneGraphNodeProperties<EigenTypes::MATH_TYPE,DIM,float> Properties;
  typedef SceneGraphNode<ParticularSceneGraphNodeProperties<EigenTypes::MATH_TYPE,DIM,float>> Parent_SceneGraphNode;
  typedef typename Properties::AffineTransformValue_::Transform Transform;

  Primitive()
    : m_shader("material")
    , m_material(*m_shader,
                 LambertianMaterial::UniformIds("light_position", "diffuse_light_color", "ambient_light_color", "ambient_lighting_proportion", "use_texture", "texture"),
                 EigenTypes::Vector3f::Zero(),  // light_position
                 Color::White(),                // diffuse_light_color
                 Color::White(),                // ambient_light_color
                 1.0f,                          // ambient_lighting_proportion
                 GL_FALSE,                      // use_texture
                 0)                             // texture
  { 
    // GLMaterial::CheckShaderForUniforms(*m_shader);
  }
  virtual ~Primitive() { }

  const GLShader &Shader () const {
    if (!m_shader) {
      throw std::runtime_error("shader member was not initialized");
    }
    return *m_shader;
  }
  const LambertianMaterial &Material () const { return m_material; }
  LambertianMaterial &Material () { return m_material; }

  typename Transform::ConstTranslationPart Translation () const { return this->LocalProperties().AffineTransform().translation(); }
  typename Transform::TranslationPart Translation () { return this->LocalProperties().AffineTransform().translation(); }
  typename Transform::ConstLinearPart LinearTransformation () const { return this->LocalProperties().AffineTransform().linear(); }
  typename Transform::LinearPart LinearTransformation () { return this->LocalProperties().AffineTransform().linear(); }

  void Draw(RenderState &render_state, const Properties &global_properties) const {
    // Set the model view (TODO: change this to not be in the RenderState, since it's tracked by DepthFirstTraverse)
    ModelView& model_view = render_state.GetModelView();
    // TODO: make a ScopeGuard for model view matrix.
    model_view.Push();
    model_view.Multiply(SquareMatrixAdaptToDim<4>(global_properties.AffineTransform().AsFullMatrix(), EigenTypes::MATH_TYPE(1)));
    MakeAdditionalModelViewTransformations(model_view);

    const GLShader &shader = Shader();
    GLShaderBindingScopeGuard bso(shader, BindFlags::BIND_AND_UNBIND); // binds shader now, unbinds upon end of scope.
    
    GLShaderMatrices::UploadUniforms(shader, model_view.Matrix(), render_state.GetProjection().Matrix(), BindFlags::NONE);
    // Material().UploadUniforms(shader, global_properties.AlphaMask(), BindFlags::NONE);
    m_material.UploadUniforms();

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
  LambertianMaterial m_material;
};

typedef Primitive<3> Primitive3;
// TODO: Deprecate the identifier "PrimitiveBase" and use Primitive3 instead.
typedef Primitive3 PrimitiveBase;
