#include <iostream>
#include "GLShader.h"
#include "GLShaderLoader.h"
#include <gtest/gtest.h>
#include "SDLController.h"

class GLShaderTest : public testing::Test {
protected:

  GLShaderTest ();

  virtual void SetUp () override;
  virtual void TearDown () override;

private:

  // SDL_Surface *m_SDL_Surface;
  // SDL_Renderer *m_SDL_Renderer;
  // SDL_GLContext m_SDL_GLContext;
  SDLController m_SDLController;
};

GLShaderTest::GLShaderTest ()
  // :
  // m_SDL_Surface(nullptr),
  // m_SDL_Renderer(nullptr)
  // m_SDL_GLContext(nullptr)
{ }

void GLShaderTest::SetUp () {
//   // Create a headless renderer

//   // Initialize SDL
//   if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
//     throw std::runtime_error(SDL_GetError());
//   }

//   // Create an SDL surface which will be used to create the SDL renderer.
//   // See https://wiki.libsdl.org/SDL_CreateRGBSurface
//   int width     = 640; // width and height are sort of arbitrarily chosen
//   int height    = 480;
//   int bit_depth = 32;
//   Uint32 rmask, gmask, bmask, amask;
// #if SDL_BYTEORDER == SDL_BIG_ENDIAN
//   rmask = 0xFF000000;
//   gmask = 0x00FF0000;
//   bmask = 0x0000FF00;
//   amask = 0x000000FF;
// #else
//   rmask = 0x000000FF;
//   gmask = 0x0000FF00;
//   bmask = 0x00FF0000;
//   amask = 0xFF000000;
// #endif
//   m_SDL_Surface = SDL_CreateRGBSurface(0, width, height, bit_depth, rmask, gmask, bmask, amask);
//   if (m_SDL_Surface == nullptr) {
//     SDL_Quit();
//     throw std::runtime_error(SDL_GetError());
//   }

//   // Create the renderer from the surface.
//   m_SDL_Renderer = SDL_CreateSoftwareRenderer(m_SDL_Surface);
//   if (m_SDL_Renderer == nullptr) {
//     SDL_FreeSurface(m_SDL_Surface);
//     m_SDL_Surface = nullptr;
//     SDL_Quit();
//     throw std::runtime_error(SDL_GetError());
//   }

//   // Create the GL context based on the renderer.
//   m_SDL_GLContext = 

  m_SDLController.Initialize();
}

void GLShaderTest::TearDown () {
  // Shut down the created things in reverse order

  m_SDLController.Shutdown();
}

// TODO: the test's setup should create a GL context (ideally with no window and as
// little overhead as possible) so that GL calls can be made.

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

TEST_F(GLShaderTest, DISABLED_RequireTypedUniformInVertexShader) {
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
    EXPECT_NO_THROW_(shader->RequireTypedUniform("test0", uniform_type));
    EXPECT_NO_THROW_(shader->RequireTypedUniform("test1", uniform_type));
  }
}

