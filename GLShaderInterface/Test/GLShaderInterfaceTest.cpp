#include "GLController.h"
#include "GLShader.h"
#include "GLShaderInterface.h"
#include "GLShaderLoader.h"
#include <gtest/gtest.h>
#include <iostream> // TEMP
#include "SDLController.h"
#include <stdexcept>

class GLShaderInterfaceTest : public testing::Test {
protected:

  GLShaderInterfaceTest () { }

  virtual void SetUp () override;
  virtual void TearDown () override;

private:

  GLController m_GLController;
  SDLController m_SDLController;
};

void GLShaderInterfaceTest::SetUp () {
  m_SDLController.Initialize();
  m_GLController.Initialize();
}

void GLShaderInterfaceTest::TearDown () {
  m_GLController.Shutdown();
  m_SDLController.Shutdown();
}

TEST_F(GLShaderInterfaceTest, ValidAttachedShader) {
  // Make sure that passing an invalid shader as the attached_shader param causes a throw.
  {
    std::shared_ptr<GLShader> invalid_shader;
    EXPECT_ANY_THROW(GLShaderInterface interface(invalid_shader));
  }

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
    EXPECT_NO_THROW(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
    EXPECT_NO_THROW(GLShaderInterface interface(valid_shader));
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

TEST_F(GLShaderInterfaceTest, CheckForTypedUniformInVertexShader) {
  // try {
    std::invalid_argument x("thingy");
    std::logic_error &y = x;
    std::invalid_argument &z = dynamic_cast<std::invalid_argument &>(y);
    std::cerr << z.what() << '\n';
    // throw std::invalid_argument("hippo");
  // } catch (const std::logic_error &e) {
    // std::cerr << "correctly caught exception " << e.what() << '\n';
  // }
/*
  const auto &type_map = GLShaderInterface::OPENGL_2_1_TYPE_MAP;
  for (auto it = type_map.begin(); it != type_map.end(); ++it) {
    // Construct a vertex shader source code with one of each type of allowable uniform.
    std::string vertex_shader_source("#version 120\n");
    vertex_shader_source += "uniform " + it->second + " test_" + it->second + ";\n";
    vertex_shader_source +=
      "void main () {\n"
      "    gl_Position = ftransform();\n"
      "    gl_FrontColor = gl_Color;\n"
      "}\n";
    // Dummy fragment shader source
    std::string fragment_shader_source(
      "#version 120\n"
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
      "}\n"
    );
    // std::cout << vertex_shader_source << '\n';

    // Construct the shader and the GLShaderInterface.
    std::shared_ptr<GLShader> valid_shader;
    ASSERT_NO_THROW(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
    GLShaderInterface interface(valid_shader);
    EXPECT_NO_THROW_(throw std::invalid_argument("blah"));
    // EXPECT_NO_THROW_(interface.CheckForTypedUniform("test_" + it->second, it->first));
  }
  */
}

