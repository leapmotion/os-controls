#include "GLShaderInterfaceMatrices.h"

#include "GLShader.h"
#include <stdexcept>

GLShaderInterfaceMatrices::GLShaderInterfaceMatrices (const std::shared_ptr<GLShader> &attached_shader)
  :
  GLShaderInterface(attached_shader)
{
  CheckForTypedUniform("projection_times_model_view_matrix", GL_FLOAT_MAT4);
  CheckForTypedUniform("model_view_matrix", GL_FLOAT_MAT4);
  CheckForTypedUniform("normal_matrix", GL_FLOAT_MAT4);
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
