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
#if 0
namespace Leap {
namespace GL {
namespace GLShaderMatrices {

void CheckShaderForUniforms (const GLShader &shader, BindFlags bind_flags) {
  GLShaderBindingScopeGuard bso(shader, bind_flags); // binds shader now if necessary, unbinds upon end of scope if necessary.
  
  shader.CheckForTypedUniform("projection_times_model_view_matrix", GL_FLOAT_MAT4, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("model_view_matrix", GL_FLOAT_MAT4, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("normal_matrix", GL_FLOAT_MAT4, VariableIs::OPTIONAL_BUT_WARN);
}

// All the matrix uniforms in the shader can be derived from the modelview and
// projection matrices.  This method derives the necessary matrices and uploads
// them into the uniforms for this shader.  The binding requirement 
void UploadUniforms (const GLShader &shader, const EigenTypes::Matrix4x4 &model_view, const EigenTypes::Matrix4x4 &projection, BindFlags bind_flags) {
  // Derive the necessary matrices, stored using floats, which is what is required by OpenGL.
  EigenTypes::Matrix4x4f projection_times_model_view_matrix((projection * model_view).cast<float>());
  EigenTypes::Matrix4x4f model_view_matrix(model_view.cast<float>()); // same as model_view, but cast to float.
  // The inverse transpose is the correct transformation in order to keep normal EigenTypes::Vectors
  // actually perpendicular to "tangent" EigenTypes::Vectors.  If model_view is an isometry, then
  // the inverse transpose is itself.
  EigenTypes::Matrix4x4f normal_matrix(model_view.inverse().transpose().cast<float>());

  GLShaderBindingScopeGuard bso(shader, bind_flags); // binds shader now if necessary, unbinds upon end of scope if necessary.
  
  // The use of COLUMN_MAJOR is because our Eigen-based Matrix4x4f typedef uses column-major data storage.
  shader.UploadUniform<GL_FLOAT_MAT4>("projection_times_model_view_matrix", projection_times_model_view_matrix, COLUMN_MAJOR);
  shader.UploadUniform<GL_FLOAT_MAT4>("model_view_matrix", model_view_matrix, COLUMN_MAJOR);
  shader.UploadUniform<GL_FLOAT_MAT4>("normal_matrix", normal_matrix, COLUMN_MAJOR);
}

} // end of namespace GLShaderMatrices
} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
#endif
