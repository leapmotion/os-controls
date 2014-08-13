#include "GLShader.h"

#include <stdexcept>
#include <sstream>

GLShader::GLShader (const std::string &vertex_shader_source, const std::string &fragment_shader_source) {
  m_vertex_shader = Compile(GL_VERTEX_SHADER, vertex_shader_source);
  m_fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_shader_source);
  m_prog = glCreateProgram();
  glAttachShader(m_prog, m_vertex_shader);
  CheckError("Attach Vertex");
  glAttachShader(m_prog, m_fragment_shader);
  CheckError("Attach Fragment");
  glLinkProgram(m_prog);
  CheckError("Link Shader");
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
  CheckError("Shader Source");
  glCompileShader(shader);
  CheckError("Compile Shader");
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  CheckError("GetShaderiv");
  if (!compiled) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string log(length, ' ');
    glGetShaderInfoLog(shader, length, &length, &log[0]);
    throw std::logic_error(log);
  }
  return shader;
}

void GLShader::CheckError(const std::string& loc) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    std::stringstream ss;
    ss << "GL error ";
    if (!loc.empty()) {
      ss << "at " << loc << ": ";
    }
    ss << "code: " << std::hex << err;
    throw std::runtime_error(ss.str());
  }
}