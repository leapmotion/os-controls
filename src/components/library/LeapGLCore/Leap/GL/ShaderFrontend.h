#pragma once

#include "Leap/GL/GLShader.h"

namespace Leap {
namespace GL {

enum class ShaderFrontendMode { IMMEDIATE, CACHED };

template <ShaderFrontendMode MODE_, typename UniformNameType_> class ShaderFrontend;

template <typename UniformNameType_, UniformNameType_ NAME_, GLenum UNIFORM_TYPE_> struct Uniform;

template <typename UniformNameType_, UniformNameType_ NAME_>
struct Uniform<UniformNameType_,NAME_,
  void Upload (const GLShader &bound_shader, )
};

template <typename UniformNameType_>
class ShaderFrontend<ShaderFrontendMode::IMMEDIATE,UniformNameType_> {
public:

  typedef UniformNameType_ UniformNameType;
  typedef ... Uniforms;

  ShaderFrontend (const GLShader &)

  template <UniformNameType_ NAME_>
  void UploadUniform ()

  void UploadUniforms (const Uniforms &uniforms) {
    ...
  }

private:

  const GLShader &m_shader;
};


/*

enum class MaterialPropertyName { DIFFUSE_COLOR, LIGHT_POSITIONS, ... };
ShaderFrontend<MaterialPropertyName,
               Uniform<DIFFUSE_COLOR,GL_FLOAT_VEC4,Rgba<GLfloat>>,
               UniformArray<LIGHT_POSITIONS,GL_FLOAT_VEC3,10,Vector3<GLfloat>>,
               ...> material(uniform_locations);

material.Set<DIFFUSE_COLOR>(Rgba<GLfloat>(1,0,0,1));

Vector3 light_positions[10] = ...; // This could be a std::array<Vector3,10> or std::vector<Vector3> of size 10.
material.Set<LIGHT_POSITIONS>(light_positions);
material.Set<LIGHT_POSITIONS>(3, 10, array_of_7_Vector3s);

for (size_t i = 0; i < 10; ++i) {
  material.Set<LIGHT_POSITIONS>(i, Vector3(expression involving i));
}

*/




} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
