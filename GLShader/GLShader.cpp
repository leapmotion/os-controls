#include "GLShader.h"

const GLintUniform UniformFunction<GLint,1>::value = glUniform1iv;
const GLintUniform UniformFunction<GLint,2>::value = glUniform2iv;
const GLintUniform UniformFunction<GLint,3>::value = glUniform3iv;
const GLintUniform UniformFunction<GLint,4>::value = glUniform4iv;
const GLfloatUniform UniformFunction<GLfloat,1>::value = glUniform1fv;
const GLfloatUniform UniformFunction<GLfloat,2>::value = glUniform2fv;
const GLfloatUniform UniformFunction<GLfloat,3>::value = glUniform3fv;
const GLfloatUniform UniformFunction<GLfloat,4>::value = glUniform4fv;

const GLfloatUniformMatrix UniformMatrixFunction<2,2>::value = glUniformMatrix2fv;
const GLfloatUniformMatrix UniformMatrixFunction<2,3>::value = glUniformMatrix2x3fv;
const GLfloatUniformMatrix UniformMatrixFunction<2,4>::value = glUniformMatrix2x4fv;
const GLfloatUniformMatrix UniformMatrixFunction<3,2>::value = glUniformMatrix3x2fv;
const GLfloatUniformMatrix UniformMatrixFunction<3,3>::value = glUniformMatrix3fv;
const GLfloatUniformMatrix UniformMatrixFunction<3,4>::value = glUniformMatrix3x4fv;
const GLfloatUniformMatrix UniformMatrixFunction<4,2>::value = glUniformMatrix4x2fv;
const GLfloatUniformMatrix UniformMatrixFunction<4,3>::value = glUniformMatrix4x3fv;
const GLfloatUniformMatrix UniformMatrixFunction<4,4>::value = glUniformMatrix4fv;

GLShader::GLShader (const std::string &vertex_shader_source, const std::string &fragment_shader_source) {
  m_vertex_shader = Compile(GL_VERTEX_SHADER, vertex_shader_source);
  m_fragment_shader = Compile(GL_FRAGMENT_SHADER, fragment_shader_source);
  m_prog = glCreateProgram();
  glAttachShader(m_prog, m_vertex_shader);
  glAttachShader(m_prog, m_fragment_shader);
  glLinkProgram(m_prog);
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
