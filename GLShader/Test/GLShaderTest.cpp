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

