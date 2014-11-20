#include "Leap/GL/GLMaterial.h"

#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLShaderBindingScopeGuard.h"

namespace Leap {
namespace GL {

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
  m_light_position(EigenTypes::Vector3f::Zero()),           // The light position is the origin.
  m_diffuse_light_color(Color::White()),        // Sensible default color.
  m_ambient_light_color(Color::White()),        // Sensible default color.
  m_ambient_lighting_proportion(0.5f),          // Half the lighting comes from ambient light.
  m_use_texture(false),                         // Texturing is disabled by default.
  m_texture_unit_index(0)                       // Default texture unit is 0, though is only used if texturing is enabled.
{ }

void GLMaterial::UploadUniforms (const GLShader &shader, float alpha_mask, BindFlags bind_flags) const {
  // Clamp the alpha mask to within the range [0,1].
  alpha_mask = std::min(std::max(alpha_mask, 0.0f), 1.0f);
  GLShaderBindingScopeGuard bso(shader, bind_flags); // binds shader now if necessary, unbinds upon end of scope if necessary.
//   shader.SetUniformf("light_position", m_light_position);
  Color diffuseColor = m_diffuse_light_color;
  Color ambientColor = m_ambient_light_color;
  diffuseColor.A() *= alpha_mask;
  ambientColor.A() *= alpha_mask;
  shader.UploadUniform<GL_FLOAT_VEC4>("diffuse_light_color", diffuseColor);
  shader.UploadUniform<GL_FLOAT_VEC4>("ambient_light_color", ambientColor);
  shader.UploadUniform<GL_FLOAT>("ambient_lighting_proportion", m_ambient_lighting_proportion);
  shader.UploadUniform<GL_BOOL>("use_texture", m_use_texture);
  shader.UploadUniform<GL_SAMPLER_2D>("texture", m_texture_unit_index);
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
