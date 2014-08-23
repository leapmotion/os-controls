#pragma once

#include "EigenTypes.h"
#include "GLShader.h"
#include <memory>

// Presents a C++ interface specifically tailored to shaders having matrix uniforms
// with the following names:
//   uniform mat4 projection_times_model_view_matrix
//   uniform mat4 model_view_matrix
//   uniform mat4 normal_matrix
// These quantities are derived from the model view matrix and the projection matrix.
class GLShaderMatrices {
public:
  
  // A valid shader must be attached to this object during construction.  If
  // attached_shader is invalid, an exception will be thrown.  The shader must have
  // the following 4x4 matrix uniforms:
  // - projection_times_model_view_matrix
  // - model_view_matrix
  // - normal_matrix
  // If these uniforms aren't present in the shader with the correct types
  // (GL_FLOAT_MAT4), an exception will be thrown.
  GLShaderMatrices (const std::shared_ptr<GLShader> &attached_shader);

  // All the matrix uniforms in the shader can be derived from the modelview and
  // projection matrices.  This method derives the necessary matrices and sets them.
  void SetMatrices (const Matrix4x4 &model_view, const Matrix4x4 &projection);
  // Sets the uniforms for the attached shader to the matrix values derived from the
  // values obtained by the last call to SetMatrices.
  void UploadUniforms (ShaderBindRequirement req) const;

private:

  std::shared_ptr<GLShader> m_attached_shader;
  Matrix4x4 m_model_view;
  Matrix4x4 m_projection;
};
