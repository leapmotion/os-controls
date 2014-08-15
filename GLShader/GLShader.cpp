#include "GLShader.h"

#include <iostream> // TEMP
#include <stdexcept>

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLShader::VarInfo
// ////////////////////////////////////////////////////////////////////////////////////////////////

GLShader::VarInfo::VarInfo (const std::string &name, GLint location, GLint size, GLenum type)
  :
  m_name(name),
  m_location(location),
  m_size(size),
  m_type(type)
{
  if (m_name.empty()) {
    throw std::invalid_argument("shader variable must have nonempty name");
  }
  if (m_location < 0) {
    throw std::invalid_argument("shader variable must have nonnegative location (index in variable list)");
  }
  if (m_size <= 0) {
    throw std::invalid_argument("shader variable must have positive size");
  }
  // There is probably no way to reasonably check the validity of type that isn't
  // specifying a hardcoded list of acceptable values.
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLShader
// ////////////////////////////////////////////////////////////////////////////////////////////////

GLShader::GLShader (const std::string &vertex_shader_source, const std::string &fragment_shader_source) {
  m_vertex_shader = Compile(GL_VERTEX_SHADER, vertex_shader_source);
  m_fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_shader_source);
  m_prog = glCreateProgram();
  glAttachShader(m_prog, m_vertex_shader);
  glAttachShader(m_prog, m_fragment_shader);
  glLinkProgram(m_prog);

  // Populate the uniform map.
  {  
    GLint active_uniforms = 0;
    glGetProgramiv(m_prog, GL_ACTIVE_UNIFORMS, &active_uniforms);
    // std::cout << "active uniforms = " << active_uniforms << '\n';
    
    GLint active_uniform_max_length = 0;
    glGetProgramiv(m_prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length);
    // std::cout << "active uniform max length = " << active_uniform_max_length << '\n';
    
    for (GLint index = 0; index < active_uniforms; ++index) {
      std::string name(active_uniform_max_length, ' ');
      GLsizei length;
      GLint size;
      GLenum type;
      glGetActiveUniform(m_prog, index, active_uniform_max_length, &length, &size, &type, &name[0]);
      name.resize(length);
      // std::cout << "uniform " << index << " -- name \"" << name << "\", size = " << size << ", type = " << std::hex << type << '\n';
      // TODO: use emplace here, then get rid of default constructor for VarInfo
      m_uniform_info_map[name] = VarInfo(name, index, size, type);
    }
  }

  // Populate the attribute map.
  {
    GLint active_attribs = 0;
    glGetProgramiv(m_prog, GL_ACTIVE_ATTRIBUTES, &active_attribs);
    // std::cout << "active attribs = " << active_attribs << '\n';
    
    GLint active_attrib_max_length = 0;
    glGetProgramiv(m_prog, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &active_attrib_max_length);
    // std::cout << "active attrib max length = " << active_attrib_max_length << '\n';
    
    for (GLint index = 0; index < active_attribs; ++index) {
      std::string name(active_attrib_max_length, ' ');
      GLsizei length;
      GLint size;
      GLenum type;
      glGetActiveAttrib(m_prog, index, active_attrib_max_length, &length, &size, &type, &name[0]);
      name.resize(length);
      // std::cout << "attrib " << index << " -- name \"" << name << "\", size = " << size << ", type = " << std::hex << type << '\n';
      // TODO: use emplace here, then get rid of default constructor for VarInfo
      m_attribute_info_map[name] = VarInfo(name, index, size, type);
    }
  }
}

GLShader::~GLShader () {
  glDeleteProgram(m_prog);
  glDeleteShader(m_vertex_shader);
  glDeleteShader(m_fragment_shader);
}

GLuint GLShader::Compile (GLuint type, const std::string &source) {
  GLuint shader = glCreateShader(type);
  const GLchar *source_ptr = source.c_str();
  glShaderSource(shader, 1, &source_ptr, NULL);
  glCompileShader(shader);
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log(length, ' ');
    glGetShaderInfoLog(shader, length, &length, &log[0]);
    throw std::logic_error(log);
  }
  return shader;
}

