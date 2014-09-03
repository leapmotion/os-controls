#include "GLMaterial.h"

#include "GLShader.h"
#include "GLShaderBindingScopeGuard.h"

void GLMaterial::CheckShaderForUniforms (const GLShader &shader) {
  // Check for the required uniforms.  Any unmet requirement will cause an exception to be thrown.
  shader.CheckForTypedUniform("light_position", GL_FLOAT_VEC3, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("diffuse_light_color", GL_FLOAT_VEC4, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("ambient_light_color", GL_FLOAT_VEC4, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("ambient_lighting_proportion", GL_FLOAT, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("use_texture", GL_BOOL, VariableIs::OPTIONAL_BUT_WARN);
  shader.CheckForTypedUniform("texture", GL_SAMPLER_2D, VariableIs::OPTIONAL_BUT_WARN);
}

GLMaterial::GLMaterial ()
  :
  // Use reasonable defaults for all the material properties.
  m_light_position(Vector3f::Zero()),           // The light position is the origin.
  m_diffuse_light_color(Color::White()),        // Sensible default color.
  m_ambient_light_color(Color::White()),        // Sensible default color.
  m_ambient_lighting_proportion(0.5f),          // Half the lighting comes from ambient light.
  m_use_texture(false),                         // Texturing is disabled by default.
  m_texture_unit_index(0)                       // Default texture unit is 0, though is only used if texturing is enabled.
{ }

void GLMaterial::UploadUniforms (const GLShader &shader, float opacityMultiplier, BindFlags bind_flags) const {
  GLShaderBindingScopeGuard bso(shader, bind_flags); // binds shader now if necessary, unbinds upon end of scope if necessary.
//   shader.SetUniformf("light_position", m_light_position);
  Color diffuseColor = m_diffuse_light_color;
  Color ambientColor = m_ambient_light_color;
  diffuseColor.A() *= opacityMultiplier;
  ambientColor.A() *= opacityMultiplier;
  shader.SetUniformf("diffuse_light_color", diffuseColor);
  shader.SetUniformf("ambient_light_color", ambientColor);
  shader.SetUniformf("ambient_lighting_proportion", m_ambient_lighting_proportion);
  shader.SetUniformi("use_texture", m_use_texture);
  shader.SetUniformi("texture", m_texture_unit_index);
}
