#include "Material.h"

#include "GLShader.h"
#include <stdexcept>

Material::Material (const std::shared_ptr<GLShader> &attached_shader)
  :
  m_attached_shader(attached_shader),
  m_shader_matrices(attached_shader),
  // Use reasonable defaults for all the material properties.
  m_diffuse_color(Color::White()),
  m_light_position(Vector3f::Zero()),          // the light position is the origin
  m_ambient_factor(0.5f),                     // half the lighting comes from ambient light
  m_use_texture(false),
  m_texture_unit_index(0)
{
  if (!m_attached_shader) {
    throw std::invalid_argument("must specify a valid attached shader");
  }
  GLShader &shader = *m_attached_shader;
  // Check for the required uniforms.  Any unmet requirement will cause an exception to be thrown.
  shader.RequireTypedUniform("diffuse_color", GL_FLOAT_VEC4);
  shader.RequireTypedUniform("light_position", GL_FLOAT_VEC3);
  shader.RequireTypedUniform("ambient_factor", GL_FLOAT);
  shader.RequireTypedUniform("use_texture", GL_BOOL);
  shader.RequireTypedUniform("texture", GL_SAMPLER_2D);
  // Ensure the material properties are uploaded.
  UploadUniforms(ShaderBindRequirement::BIND_AND_UNBIND);
}

void Material::UploadUniforms (ShaderBindRequirement req) {
  if (req == ShaderBindRequirement::BIND_AND_UNBIND) {
    m_attached_shader->Bind();
  }
  m_shader_matrices.UploadUniforms(ShaderBindRequirement::DONT_BIND_OR_UNBIND);
  GLShader &shader = *m_attached_shader;
  // TODO: add "dirty cache" variables so that only changed uniforms are uploaded.
  shader.SetUniformf("diffuse_color", m_diffuse_color);
  shader.SetUniformf("light_position", m_light_position);
  shader.SetUniformf("ambient_factor", m_ambient_factor);
  shader.SetUniformi("use_texture", m_use_texture);
  shader.SetUniformi("texture", m_texture_unit_index);
  if (req == ShaderBindRequirement::BIND_AND_UNBIND) {
    m_attached_shader->Unbind();
  }
}
