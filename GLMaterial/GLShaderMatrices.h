#pragma once

#include "EigenTypes.h"
#include "GLShader.h"
#include "ScopeGuard.h"

// Presents an interface specifically tailored to shaders having matrix uniforms
// with the following names:
//   uniform mat4 projection_times_model_view_matrix
//   uniform mat4 model_view_matrix
//   uniform mat4 normal_matrix
// These quantities are derived from the model view matrix and the projection matrix.

namespace GLShaderMatrices {
  
// The shader must have the following 4x4 matrix uniforms:
// - projection_times_model_view_matrix
// - model_view_matrix
// - normal_matrix
// If these uniforms aren't present in the shader with the correct types (GL_FLOAT_MAT4),
// an exception will be thrown.  The shader may be bound and/or unbound, depending on the
// value of bind_flags.
void CheckShaderForUniforms (const GLShader &shader, BindFlags bind_flags);

// All the matrix uniforms in the shader can be derived from the modelview and
// projection matrices.  This method derives the necessary matrices and uploads
// them into the uniforms for this shader.  The binding requirement 
void UploadUniforms (const GLShader &shader, const EigenTypes::Matrix4x4 &model_view, const EigenTypes::Matrix4x4 &projection, BindFlags bind_flags);

} // end of namespace GLShaderMatrices

