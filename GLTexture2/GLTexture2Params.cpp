#include "GLTexture2Params.h"

#include <stdexcept>

GLTexture2Params::GLTexture2Params (GLsizei width, GLsizei height)
  :
  m_target(DEFAULT_TARGET),
  m_internal_format(DEFAULT_INTERNAL_FORMAT)
{ 
  m_size[0] = width;
  m_size[1] = height;
}

GLfloat GLTexture2Params::TexParameterf (GLenum pname) const {
  GLTexParameterfMap::const_iterator it = m_tex_parameter_f.find(pname);
  if (it == m_tex_parameter_f.end()) {
    throw std::domain_error("specified GLfloat-valued texture parameter not found and/or specified");
  }
  return it->second;
}

GLint GLTexture2Params::TexParameteri (GLenum pname) const {
  GLTexParameteriMap::const_iterator it = m_tex_parameter_i.find(pname);
  if (it == m_tex_parameter_i.end()) {
    throw std::domain_error("specified GLint-valued texture parameter not found and/or specified");
  }
  return it->second;
}

void GLTexture2Params::SetTexParameterf (GLenum pname, GLfloat value) {
  if (m_tex_parameter_i.find(pname) != m_tex_parameter_i.end()) {
    throw std::domain_error("specified GLfloat-valued texture parameter already set in the GLint-valued parameters");
  }
  m_tex_parameter_f[pname] = value;
}
void GLTexture2Params::SetTexParameteri (GLenum pname, GLint value) {
  if (m_tex_parameter_f.find(pname) != m_tex_parameter_f.end()) {
    throw std::domain_error("specified GLint-valued texture parameter already set in the GLfloat-valued parameters");
  }
  m_tex_parameter_i[pname] = value;
}

