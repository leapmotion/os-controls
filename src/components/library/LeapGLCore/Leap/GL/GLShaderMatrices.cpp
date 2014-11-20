#include "Leap/GL/GLShaderMatrices.h"

#include "Leap/GL/GLShaderBindingScopeGuard.h"

ShaderMatrices::ShaderMatrices (const GLShader &shader,
                                const std::string &projection_times_model_view_matrix_id,
                                const std::string &model_view_matrix_id,
                                const std::string &normal_matrix_id)
  : m_frontend(shader, Frontend::UniformIds(projection_times_model_view_matrix_id, model_view_matrix_id, normal_matrix_id))
{ }

void ShaderMatrices::SetMatrices (const EigenTypes::Matrix4x4 &model_view, const EigenTypes::Matrix4x4 &projection) {
  // Derive the necessary matrices, stored using floats, which is what is required by OpenGL.
  m_frontend.Uniform<ShaderMatrix::PROJECTION_TIMES_MODEL_VIEW>() = (projection * model_view).cast<float>();
  m_frontend.Uniform<ShaderMatrix::MODEL_VIEW>() = model_view.cast<float>(); // same as model_view, but cast to float.
  // The inverse transpose is the correct transformation in order to keep normal EigenTypes::Vectors
  // actually perpendicular to "tangent" EigenTypes::Vectors.  If model_view is an isometry, then
  // the inverse transpose is itself.
  m_frontend.Uniform<ShaderMatrix::NORMAL>() = model_view.inverse().transpose().cast<float>();
}

void ShaderMatrices::UploadUniforms () {
  m_frontend.UploadUniforms();
}
