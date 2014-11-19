#include <array>
#include <iostream>
#include "GLTestFramework.h"
#include <gtest/gtest.h>
#include "Leap/GL/ShaderFrontend.h"

// Convenience macro for annotated printing the value of variables.
#define FMT(x) #x << " = " << (x)

class ShaderFrontendTest : public GLTestFramework_Headless { };

std::shared_ptr<GLShader> CreateShaderWithUniform (const std::string &name, const std::string &uniform_type_name, const std::string &glsl_version) {
  // Disallow the sampler types for now.
  assert(uniform_type_name.find("sampler") == std::string::npos);

  // Construct a vertex shader source code with one of each type of allowable uniform.
  std::string vertex_shader_source("#version " + glsl_version + "\n");
  vertex_shader_source += "uniform " + uniform_type_name + " " + name + ";\n";
  vertex_shader_source += "uniform " + uniform_type_name + " " + name + "_;\n";
  vertex_shader_source +=
    "void main () {\n"
    "    bool condition = " + name + " == " + name + "_;\n" // This dumb indirection is to try to get around unused uniforms maybe being compiled out of the shader.
    "    if (condition) {\n"
    "        gl_Position = ftransform();\n"
    "        gl_FrontColor = gl_Color;\n"
    "    }\n"
    "}\n";
  // Dummy fragment shader source
  std::string fragment_shader_source(
    "#version " + glsl_version + "\n"
    "void main () {\n"
    "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n"
  );
  std::cout << vertex_shader_source << '\n';
  return std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source);
}

enum class UniformName { THINGY };
template <UniformName NAME_, GLenum GL_TYPE_, typename CppType_> using ThingyUniform = Leap::GL::Uniform<UniformName,NAME_,GL_TYPE_,CppType_>;

TEST_F(ShaderFrontendTest, Test_float) {
  auto shader = CreateShaderWithUniform("thingy", "float", "120");
  typedef ShaderFrontend<UniformName,
                         ThingyUniform<UniformName::THINGY,GL_FLOAT,float>> Frontend;
  Frontend frontend(*shader, Frontend::UniformLocations(shader->LocationOfUniform("thingy")));
  shader->Bind();
  frontend.Set<UniformName::THINGY>(4.566f);
  shader->Unbind();
}

TEST_F(ShaderFrontendTest, Test_vec2) {
  auto shader = CreateShaderWithUniform("thingy", "vec2", "120");
  typedef ShaderFrontend<UniformName,
                         ThingyUniform<UniformName::THINGY,GL_FLOAT_VEC2,std::array<float,2>>> Frontend;
  Frontend frontend(*shader, Frontend::UniformLocations(shader->LocationOfUniform("thingy")));
  shader->Bind();
  frontend.Set<UniformName::THINGY>({{4.56f, 88.0f}});
  // frontend.Set<UniformName::THINGY>({{4.56f, 88.0f, 100.0f}}); // This should cause a compile error.
  shader->Unbind();
}

TEST_F(ShaderFrontendTest, Test_vec3) {
  struct V {
    float x,y,z;
    V () { }
    V (float x_, float y_, float z_) : x(x_), y(y_), z(z_) { }
  };
  auto shader = CreateShaderWithUniform("thingy", "vec3", "120");
  typedef ShaderFrontend<UniformName,
                         ThingyUniform<UniformName::THINGY,GL_FLOAT_VEC3,V>> Frontend;
  Frontend frontend(*shader, Frontend::UniformLocations(shader->LocationOfUniform("thingy")));
  shader->Bind();
  frontend.Set<UniformName::THINGY>(V(4.56f, 88.0f, -1.02f));
  frontend.Set<UniformName::THINGY>(4.56f, 88.0f, -1.02f);
  shader->Unbind();
}

  /*
  { GL_FLOAT, "float" },
  { GL_FLOAT_VEC2, "vec2" },
  { GL_FLOAT_VEC3, "vec3" },
  { GL_FLOAT_VEC4, "vec4" },
  { GL_INT, "int" },
  { GL_INT_VEC2, "ivec2" },
  { GL_INT_VEC3, "ivec3" },
  { GL_INT_VEC4, "ivec4" },
  { GL_BOOL, "bool" },
  { GL_BOOL_VEC2, "bvec2" },
  { GL_BOOL_VEC3, "bvec3" },
  { GL_BOOL_VEC4, "bvec4" },
  { GL_FLOAT_MAT2, "mat2" },
  { GL_FLOAT_MAT3, "mat3" },
  { GL_FLOAT_MAT4, "mat4" },
  { GL_FLOAT_MAT2x3, "mat2x3" },
  { GL_FLOAT_MAT2x4, "mat2x4" },
  { GL_FLOAT_MAT3x2, "mat3x2" },
  { GL_FLOAT_MAT3x4, "mat3x4" },
  { GL_FLOAT_MAT4x2, "mat4x2" },
  { GL_FLOAT_MAT4x3, "mat4x3" },
  */
