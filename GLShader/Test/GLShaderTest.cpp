#include <iostream>
#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTestFramework.h"
#include <gtest/gtest.h>

class GLShaderTest : public GLTestFramework_Headless { };

TEST_F(GLShaderTest, CompileAndLink) {
  std::string vertex_shader_source(
    "void main () {\n"
    "    gl_Position = ftransform();\n"
    "    gl_FrontColor = gl_Color;\n"
    "}\n"
  );
  std::string fragment_shader_source(
    "void main () {\n"
    "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
    "}\n"
  );
  EXPECT_NO_THROW(auto valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
}

TEST_F(GLShaderTest, CompileSuccessfully) {
  // Make sure that passing a valid shader as the attached_shader param doesn't throw.
  {
    std::string vertex_shader_source(
      "void main () {\n"
      "    gl_Position = ftransform();\n"
      "    gl_FrontColor = gl_Color;\n"
      "}\n"
    );
    std::string fragment_shader_source(
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
      "}\n"
    );
    std::shared_ptr<GLShader> valid_shader;
    ASSERT_NO_THROW(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
  }
}

TEST_F(GLShaderTest, CompileUnsuccessfully) {
  // Make sure that passing a valid shader as the attached_shader param doesn't throw.
  {
    std::string vertex_shader_source(
      "void main () {\n"
      "    this in an intentional compile error\n"
      "    gl_FrontColor = gl_Color;\n"
      "}\n"
    );
    std::string fragment_shader_source(
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
      "}\n"
    );
    std::shared_ptr<GLShader> valid_shader;
    ASSERT_ANY_THROW(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
  }
}

#define EXPECT_NO_THROW_(statement) \
  try { \
    statement; \
  } catch (const std::exception &e) { \
    EXPECT_TRUE(false) << "Expected no exception thrown, but one was thrown: \"" << e.what() << '\"'; \
  } catch (...) { \
    EXPECT_TRUE(false) << "Expected no exception thrown, but one was thrown (no message available)"; \
  }

TEST_F(GLShaderTest, DISABLED_SetTypedUniformCheckInVertexShader) {
  const auto &type_map = GLShader::OPENGL_2_1_UNIFORM_TYPE_MAP;
  for (auto it = type_map.begin(); it != type_map.end(); ++it) {
    GLenum uniform_type = it->first;
    const std::string &uniform_type_name = it->second;
//     std::string uniform_variable_name("test_" + uniform_type_name);
    // Construct a vertex shader source code with one of each type of allowable uniform.
    std::string vertex_shader_source("#version 120\n");
    vertex_shader_source += "uniform " + uniform_type_name + " test0;\n";
    vertex_shader_source += "uniform " + uniform_type_name + " test1;\n";
    vertex_shader_source +=
      "void main () {\n"
      "    bool condition = test0 == test1;\n" // This dumb indirection is to try to get around unused uniforms maybe being compiled out of the shader.
      "    if (condition) {\n"
      "        gl_Position = ftransform();\n"
      "        gl_FrontColor = gl_Color;\n"
      "    }\n"
      "}\n";
    // Dummy fragment shader source
    std::string fragment_shader_source(
      "#version 120\n"
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
      "}\n"
    );
    std::cout << vertex_shader_source << '\n';

    // Construct the shader and the GLShaderInterface.
    std::shared_ptr<GLShader> shader;
    ASSERT_NO_THROW(shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
    EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::REQUIRED));
    EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::REQUIRED));
  }
}

