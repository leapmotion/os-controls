#pragma once

#include "Color.h"
#include "EigenTypes.h"
#include "GLShaderMatrices.h"
#include <memory>

class GLShader;

// Presents a C++ interface specifically tailored to the "material" shader having
// particular uniforms:
//   uniform vec4 diffuse_color;
//   uniform vec3 light_position;
//   uniform float ambient_factor;
//   uniform bool use_texture;
//   uniform sampler2D texture;
// Setting these uniforms is done through specialized modifiers in this class.
// This class also has a GLShaderInterfaceMatrices, which requires the existence of
// particular matrix uniforms.  See GLShaderInterfaceMatrices.
class Material {
public:

  // A valid shader must be attached to this object during construction.  If
  // attached_shader is invalid, an exception will be thrown.  The shader must have
  // uniforms described above.  If these uniforms aren't present in the shader with
  // the correct types, an exception will be thrown.
  Material (const std::shared_ptr<GLShader> &attached_shader);

  // Frontend for the matrices of this shader.  See GLShaderMatrices.
  void SetMatrices (const Matrix4x4 &model_view, const Matrix4x4 &projection) { m_shader_matrices.SetMatrices(model_view, projection); }
  // Modifiers for the uniforms of a material.
  void SetDiffuseColor (const Color &diffuse_color) { m_diffuse_color = diffuse_color; }
  void SetLightPosition (const Vector3f &light_position) { m_light_position = light_position; }
  void SetAmbientFactor (float ambient_factor) { m_ambient_factor = ambient_factor; }
  void SetUseTexture (bool use_texture) { m_use_texture = use_texture; }
  void SetTexture (GLint texture_unit_index) { m_texture_unit_index = texture_unit_index; }

  void UploadUniforms (ShaderBindRequirement req);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
private:

  std::shared_ptr<GLShader> m_attached_shader;
  GLShaderMatrices m_shader_matrices; // TODO: this should really be a part of a GLMesh component, not Material
  Color m_diffuse_color;
  Vector3f m_light_position;
  float m_ambient_factor;
  bool m_use_texture;
  GLint m_texture_unit_index;
};
