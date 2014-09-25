#include "PrimitiveBase.h"
#include "RenderState.h"

#include "GLShader.h"
#include "GLShaderBindingScopeGuard.h"
#include "GLShaderLoader.h"
#include <stack>

PrimitiveBase::PrimitiveBase()
  :
  m_shader("material")
{
  GLMaterial::CheckShaderForUniforms(*m_shader);
}

void PrimitiveBase::DrawSceneGraph(const PrimitiveBase& rootNode, RenderState &render_state) {
  // TODO: the existing model view matrix can be inputted as the initial state of global_properties
  // in the call to DepthFirstTraverse.
  rootNode.DepthFirstTraverse<PrimitiveBase>([&render_state](const PrimitiveBase &node, const Properties &global_properties) {
    node.Draw(render_state, global_properties);
  });
}

Matrix3x3 PrimitiveBase::SquashStretchTransform(const Vector3& velocity, const Vector3& viewDirection, double speedDenom) {
  // compute velocity magnitude and direction
  const double speed = velocity.norm();
  static const double EPSILON = 0.0001;
  if (speedDenom < EPSILON || speed < EPSILON*speedDenom) {
    return Matrix3x3::Identity();
  }
  const Vector3 direction = velocity / speed;

  // compute stretch and squash multipliers (volume preserving)
  const double stretch = 1.0 + std::min(1.0, speed / speedDenom);
  const double squash = std::sqrt(1.0 / stretch);

  // compute velocity basis and its inverse for rotation
  Matrix3x3 velocityBasis(Matrix3x3::Identity());
  velocityBasis.col(0) = direction;
  velocityBasis.col(1) = direction.cross(viewDirection);
  velocityBasis.col(2) = viewDirection;
  const Matrix3x3 velocityBasisInv = velocityBasis.inverse();

  // compute scale matrix for deformation
  const Matrix3x3 scaleMatrix = Vector3(stretch, squash, squash).asDiagonal();

  // undo rotation, deform, then rotate back
  return velocityBasis * scaleMatrix * velocityBasisInv;
}

void PrimitiveBase::Draw(RenderState &render_state, const Properties &properties) const {
  // Set the model view (TODO: change this to not be in the RenderState, since it's tracked by DepthFirstTraverse)
  ModelView& model_view = render_state.GetModelView();
  // TODO: make a ScopeGuard for model view matrix.
  model_view.Push();
  model_view.Multiply(properties.AffineTransform().AsFullMatrix());
  MakeAdditionalModelViewTransformations(model_view);

  const GLShader &shader = Shader();
  GLShaderBindingScopeGuard bso(shader, BindFlags::BIND_AND_UNBIND); // binds shader now, unbinds upon end of scope.
  
  GLShaderMatrices::UploadUniforms(shader, model_view.Matrix(), render_state.GetProjection().Matrix(), BindFlags::NONE);
  Material().UploadUniforms(shader, properties.AlphaMask(), BindFlags::NONE);

  DrawContents(render_state);

  model_view.Pop(); // TODO: once the ScopeGuard for the model view matrix is created, this goes away.
}
