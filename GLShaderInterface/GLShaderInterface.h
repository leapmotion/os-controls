#pragma once

#include "gl_glext_glu.h"
#include <memory>
#include <unordered_map>

class GLShader;

// Base class for "shader interface" classes.  This does the work of checking
// for (or handling the lack of) required uniforms.
class GLShaderInterface {
public:

  // A valid shader must be attached to this object during construction.  If
  // attached_shader is invalid, an exception will be thrown.
  GLShaderInterface (const std::shared_ptr<GLShader> &attached_shader);
  virtual ~GLShaderInterface () { }

  // Checks for the uniform with given name and type.  If that typed uniform is
  // not found, an exception will be thrown.
  void CheckForTypedUniform (const std::string &name, GLenum type);

  static const std::unordered_map<GLenum,std::string> OPENGL_2_1_TYPE_MAP;
  static const std::unordered_map<GLenum,std::string> OPENGL_3_3_TYPE_MAP;

private:

  // Returns (enum_name_string, type_name_string) for the given uniform type.  Throws an
  // error if that type is not a uniform type.
  static const std::string &UniformTypeString (GLenum type);

  std::shared_ptr<GLShader> m_attached_shader;
};
