#include <array>
#include <iostream>
#include "GLTestFramework.h"
#include <gtest/gtest.h>
#include "Leap/GL/GLShader.h"
#include <memory>

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
  EXPECT_NO_THROW_(auto valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
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
    ASSERT_NO_THROW_(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
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

void SetTypedUniformCheckInVertexShader (const std::map<GLenum,std::string> &uniform_type_map, const std::string &glsl_version) {
  for (auto it = uniform_type_map.begin(); it != uniform_type_map.end(); ++it) {
    GLenum uniform_type = it->first;
    const std::string &uniform_type_name = it->second;
    // Skip the sampler types for now.
    if (uniform_type_name.find("sampler") == std::string::npos) {
      // Construct a vertex shader source code with one of each type of allowable uniform.
      std::string vertex_shader_source("#version " + glsl_version + "\n");
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
      ASSERT_NO_THROW_(shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::REQUIRED));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::REQUIRED));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::OPTIONAL_NO_WARN));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::OPTIONAL_NO_WARN));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::OPTIONAL_BUT_WARN));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::OPTIONAL_BUT_WARN));
    }
  }
}

TEST_F(GLShaderTest, SetTypedUniformCheckInVertexShader_OpenGL_2_1) {
  SetTypedUniformCheckInVertexShader(GLShader::OPENGL_2_1_UNIFORM_TYPE_MAP, "120");
}

TEST_F(GLShaderTest, DISABLED_SetTypedUniformCheckInVertexShader_OpenGL_3_3) {
  SetTypedUniformCheckInVertexShader(GLShader::OPENGL_3_3_UNIFORM_TYPE_MAP, "330");
}

class GLShaderTest_Visible : public GLTestFramework_Visible { };

void RenderRectangle () {
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
    
  static const GLuint VERTEX_COUNT = 4;
  const GLfloat param = 0.7f;
  const GLfloat vertex_array[VERTEX_COUNT*2] = {
    -param, -param,
     param, -param,
     param,  param,
    -param,  param
  };
  static const GLfloat TEXTURE_COORD_ARRAY[VERTEX_COUNT*2] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
  };

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, vertex_array);
  glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
}

TEST_F(GLShaderTest_Visible, Vec2UniformSettingEquivalence) {
  std::string vertex_shader_source(
    "#version 120\n"
    "void main () {\n"
    "    gl_Position = ftransform();\n"
    "    gl_FrontColor = gl_Color;\n"
    "}\n"
  );
  std::string fragment_shader_source(
    "uniform vec2 thingy0;\n"
    "uniform vec2 thingy1;\n"
    "void main () {\n"
    "    gl_FragColor = vec4(thingy0.x == thingy1.x ? 1 : 0,\n"
    "                        thingy0.y == thingy1.y ? 1 : 0,\n"
    "                        1,\n"
    "                        1);\n"
    "}\n"
  );

  // This will throw if there is an error.
  GLShader shader(vertex_shader_source, fragment_shader_source);
  shader.Bind();
  std::array<float,2> v{{2.0f, 3.0f}};
  glUniform2f(shader.LocationOfUniform("thingy0"), v[0], v[1]);
  glUniform2fv(shader.LocationOfUniform("thingy1"), 1, &v[0]);
  RenderRectangle();
  shader.Unbind();

  // Finish the frame before delaying. 
  EndFrame();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

TEST_F(GLShaderTest_Visible, Vec3UniformSettingEquivalence) {
  std::string vertex_shader_source(
    "#version 120\n"
    "void main () {\n"
    "    gl_Position = ftransform();\n"
    "    gl_FrontColor = gl_Color;\n"
    "}\n"
  );
  std::string fragment_shader_source(
    "uniform vec3 thingy0;\n"
    "uniform vec3 thingy1;\n"
    "void main () {\n"
    "    gl_FragColor = vec4(thingy0.x == thingy1.x ? 1 : 0,\n"
    "                        thingy0.y == thingy1.y ? 1 : 0,\n"
    "                        thingy0.z == thingy1.z ? 1 : 0,\n"
    "                        1);\n"
    "}\n"
  );

  // This will throw if there is an error.
  GLShader shader(vertex_shader_source, fragment_shader_source);
  shader.Bind();
  std::array<float,3> v{{2.0f, 3.0f, 4.0f}};
  glUniform3f(shader.LocationOfUniform("thingy0"), v[0], v[1], v[2]);
  glUniform3fv(shader.LocationOfUniform("thingy1"), 1, &v[0]);
  RenderRectangle();
  shader.Unbind();

  // Finish the frame before delaying. 
  EndFrame();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

