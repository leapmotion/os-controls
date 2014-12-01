#include "ExampleLayer.h"

#include <cmath>

#include "GLShaderLoader.h"
#include "GLTexture2Loader.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLTexture2.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "SDLController.h"

ExampleLayer::ExampleLayer ()
  :
  m_time(0)
{
  m_shader = Resource<GLShader>("dummy");
  m_texture = Resource<GLTexture2>("public_domain_astronomy_1.jpg");
}

ExampleLayer::~ExampleLayer () { }

void ExampleLayer::Update (TimeDelta real_time_delta) {
  m_time += real_time_delta;
}

void ExampleLayer::Render (TimeDelta real_time_delta) const {
  // render dummy geometry just as a test
  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);

  if (m_shader) {
    static const GLuint VERTEX_COUNT = 4;
    const GLfloat param = static_cast<GLfloat>(0.1f * std::sin(2.0f*m_time) + 0.9f);
    const GLfloat vertex_array[VERTEX_COUNT*2] = {
      -param, -param,
       param, -param,
       param,  param,
      -param,  param
    };

    m_shader->Bind();
    glVertexPointer(2, GL_FLOAT, 0, vertex_array);
    glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
    m_shader->Unbind();
  }

  // render dummy texture just as a test
  if (m_texture) {
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    static const GLuint VERTEX_COUNT = 4;
    const GLfloat param = static_cast<GLfloat>(0.1f * std::sin(3.0f*m_time) + 0.7f);
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
    m_texture->Bind();
    glVertexPointer(2, GL_FLOAT, 0, vertex_array);
    glTexCoordPointer(2, GL_FLOAT, 0, TEXTURE_COORD_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
    m_texture->Unbind();
  }
}
