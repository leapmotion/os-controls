#include "GLShader.h"

#include <iostream> // TEMP
#include <stdexcept>

GLShader::GLShader (const std::string &vertex_shader_source, const std::string &fragment_shader_source) {
  m_vertex_shader = Compile(GL_VERTEX_SHADER, vertex_shader_source);
  m_fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_shader_source);
  m_prog = glCreateProgram();
  glAttachShader(m_prog, m_vertex_shader);
  glAttachShader(m_prog, m_fragment_shader);
  glLinkProgram(m_prog);
  
  GLint active_uniforms = 0;
  glGetProgramiv(m_prog, GL_ACTIVE_UNIFORMS, &active_uniforms);
  std::cout << "active uniforms = " << active_uniforms << '\n';
  
  GLint active_uniform_max_length = 0;
  glGetProgramiv(m_prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length);
  std::cout << "active uniform max length = " << active_uniform_max_length << '\n';
  
  for (GLint i = 0; i < active_uniforms; ++i) {
    std::string buffer(active_uniform_max_length, ' ');
    GLsizei length;
    GLint size;
    GLenum type;
    glGetActiveUniform(m_prog, i, active_uniform_max_length, &length, &size, &type, &buffer[0]);
    buffer.resize(length);
    std::cout << "uniform " << i << " : " << buffer << '\n';
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
