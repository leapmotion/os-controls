#pragma once

#include "Color.h"
#include "EigenTypes.h"
#include "GLShader.h"
#include <memory>

class GLShader;

// Presents a C++ interface specifically tailored to the "material" shader having
// particular uniforms:
//   uniform vec3 light_position;
//   uniform vec4 diffuse_light_color;
//   uniform vec4 ambient_light_color;
//   uniform float ambient_lighting_proportion;
//   uniform bool use_texture;
//   uniform sampler2D texture;
// Material should only affect fragment shading.
// Setting these uniforms is done through specialized modifiers in this class.
class Material {
public:

  // A valid shader must be attached to this object during construction.  If
  // attached_shader is invalid, an exception will be thrown.  The shader must have
  // uniforms described above.  If these uniforms aren't present in the shader with
  // the correct types, an exception will be thrown.
  Material (const std::shared_ptr<GLShader> &attached_shader);

  const std::shared_ptr<GLShader> &AttachedShader () const { return m_attached_shader; }
  
  // Modifiers for the uniforms of a material.
  void SetLightPosition (const Vector3f &p) { m_light_position = p; } // TODO: move this elsewhere -- it doesn't belong here
  void SetDiffuseLightColor (const Color &c) { m_diffuse_light_color = c; }
  void SetAmbientLightColor (const Color &c) { m_ambient_light_color = c; }
  void SetAmbientLightingProportion (float f) { m_ambient_lighting_proportion = f; }
  void SetUseTexture (bool b) { m_use_texture = b; }
  void SetTexture (GLint texture_unit_index) { m_texture_unit_index = texture_unit_index; }

  void UploadUniforms (ShaderBindRequirement req) const;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
private:

  std::shared_ptr<GLShader> m_attached_shader;
  Vector3f m_light_position;
  Color m_diffuse_light_color;
  Color m_ambient_light_color;
  float m_ambient_lighting_proportion;
  bool m_use_texture;
  GLint m_texture_unit_index;
};
