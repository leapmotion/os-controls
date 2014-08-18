#include "GLShaderInterfaceMatrices.h"

#include "GLShader.h"
#include <stdexcept>

GLShaderInterfaceMatrices::GLShaderInterfaceMatrices (const std::shared_ptr<GLShader> &attached_shader)
  :
  m_attached_shader(attached_shader)
{
  if (!m_attached_shader) {
    throw std::invalid_argument("must specify a valid attached shader");
  }
  if (!m_attached_shader->HasUniform("projection_times_model_view_matrix") ||
      m_attached_shader->UniformInfo("projection_times_model_view_matrix").Type() != GL_FLOAT_MAT4) {
    throw std::invalid_argument("attached shader must have a uniform mat4 projection_times_model_view_matrix");
  }
  if (!m_attached_shader->HasUniform("model_view_matrix") ||
      m_attached_shader->UniformInfo("model_view_matrix").Type() != GL_FLOAT_MAT4) {
    throw std::invalid_argument("attached shader must have a uniform mat4 model_view_matrix");
  }
  if (!m_attached_shader->HasUniform("normal_matrix") ||
      m_attached_shader->UniformInfo("normal_matrix").Type() != GL_FLOAT_MAT4) {
    throw std::invalid_argument("attached shader must have a uniform mat4 normal_matrix");
  }
}

void GLShaderInterfaceMatrices::SetMatrices (const Matrix4x4 &model_view, const Matrix4x4 &projection) {
  // Derive the necessary matrices, stored using floats, which is what is required by OpenGL.
  Matrix4x4f projection_times_model_view_matrix((projection * model_view).cast<float>());
  Matrix4x4f model_view_matrix(model_view.cast<float>()); // same as model_view, but cast to float.
  // The inverse transpose is the correct transformation in order to keep normal vectors
  // actually perpendicular to "tangent" vectors.  If model_view is an isometry, then
  // the inverse transpose is model_view.
  Matrix4x4f normal_matrix(model_view.inverse().transpose().cast<float>());

  // TODO: figure out a way to bind only if necessary?
  m_attached_shader->Bind();
  m_attached_shader->SetUniformMatrixf<4,4>("projection_times_model_view_matrix", projection_times_model_view_matrix, COLUMN_MAJOR);
  m_attached_shader->SetUniformMatrixf<4,4>("model_view_matrix", model_view_matrix, COLUMN_MAJOR);
  m_attached_shader->SetUniformMatrixf<4,4>("normal_matrix", normal_matrix, COLUMN_MAJOR);
  m_attached_shader->Unbind();
}
