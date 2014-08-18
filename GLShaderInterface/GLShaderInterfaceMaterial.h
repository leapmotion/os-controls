#pragma once

#include <memory>

class GLShader;

// Presents a C++ interface specifically tailored to the "material" shader having
// particular uniforms:
// - uniform vec4 diffuseColor;
// - uniform vec3 lightPosition;
// - uniform float ambientFactor;
// Setting these uniforms is done through specialized modifiers in this class.
class GLShaderInterfaceMaterial {
public:

  // A valid shader must be attached to this object during construction.  If
  // attached_shader is invalid, an exception will be thrown.  The shader must have
  // uniforms described above.  If these uniforms aren't present in the shader with
  // the correct types, an exception will be thrown.
  GLShaderInterfaceMaterial (const std::shared_ptr<GLShader> &attached_shader);

  // TODO:

private:

  std::shared_ptr<GLShader> m_attached_shader;
};
