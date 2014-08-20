#include "GLShaderMatrices.h"

#include <stdexcept>

GLShaderMatrices::GLShaderMatrices (const std::shared_ptr<GLShader> &attached_shader)
  :
  m_attached_shader(attached_shader)
{
  if (!m_attached_shader) {
    throw std::invalid_argument("must specify a valid attached shader");
  }
  m_attached_shader->RequireTypedUniform("projection_times_model_view_matrix", GL_FLOAT_MAT4);
  m_attached_shader->RequireTypedUniform("model_view_matrix", GL_FLOAT_MAT4);
  m_attached_shader->RequireTypedUniform("normal_matrix", GL_FLOAT_MAT4);
  m_model_view.setIdentity();
  m_projection.setIdentity();
}

void GLShaderMatrices::SetMatrices (const Matrix4x4 &model_view, const Matrix4x4 &projection) {
  // TODO: check that the matrices are invertible (at least that model_view is).
  m_model_view = model_view;
  m_projection = projection;
}

void GLShaderMatrices::UploadUniforms (ShaderBindRequirement req) const
{
  // Derive the necessary matrices, stored using floats, which is what is required by OpenGL.
  Matrix4x4f projection_times_model_view_matrix((m_projection * m_model_view).cast<float>());
  Matrix4x4f model_view_matrix(m_model_view.cast<float>()); // same as model_view, but cast to float.
  // The inverse transpose is the correct transformation in order to keep normal vectors
  // actually perpendicular to "tangent" vectors.  If model_view is an isometry, then
  // the inverse transpose is itself.
  Matrix4x4f normal_matrix(m_model_view.inverse().transpose().cast<float>());

  if (req == ShaderBindRequirement::BIND_AND_UNBIND) {
    m_attached_shader->Bind();
  }
  // The use of COLUMN_MAJOR is because our Eigen-based Matrix4x4f typedef uses column-major data storage.
  m_attached_shader->SetUniformMatrixf<4,4>("projection_times_model_view_matrix", projection_times_model_view_matrix, COLUMN_MAJOR);
  m_attached_shader->SetUniformMatrixf<4,4>("model_view_matrix", model_view_matrix, COLUMN_MAJOR);
  m_attached_shader->SetUniformMatrixf<4,4>("normal_matrix", normal_matrix, COLUMN_MAJOR);
  if (req == ShaderBindRequirement::BIND_AND_UNBIND) {
    m_attached_shader->Unbind();
  }
}
