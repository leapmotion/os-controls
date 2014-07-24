#include "ExampleLayer.h"

#include "GLController.h"
#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTexture2.h"
#include "GLTexture2Loader.h"
#include "Resource.h"


ExampleLayer::ExampleLayer () {
  // m_shader = Resource<GLShader>("dummy"); // would need to add these resources to the correct location.
  // m_texture = Resource<GLTexture2>("redwood.png");
}

ExampleLayer::~ExampleLayer () { }

void ExampleLayer::Update (TimeDelta realTimeDelta) {

}

void ExampleLayer::Render (TimeDelta realTimeDelta) const {
  // render dummy geometry just as a test
  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  {
    static GLuint const VERTEX_COUNT = 4;
    static float const VERTEX_ARRAY[VERTEX_COUNT*2] = {
      -0.8f, -0.8f,
       0.8f, -0.8f,
       0.8f,  0.8f,
      -0.8f,  0.8f
    };

    m_shader->Bind();
    glVertexPointer(2, GL_FLOAT, 0, VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
    m_shader->Unbind();
  }

  // render dummy texture just as a test
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  {
    static GLuint const VERTEX_COUNT = 4;
    static float const VERTEX_ARRAY[VERTEX_COUNT*2] = {
      -0.6f, -0.6f,
       0.6f, -0.6f,
       0.6f,  0.6f,
      -0.6f,  0.6f
    };
    static float const TEXTURE_COORD_ARRAY[VERTEX_COUNT*2] = {
      0.0f, 0.0f,
      1.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f
    };
    m_texture->Bind();
    glVertexPointer(2, GL_FLOAT, 0, VERTEX_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, TEXTURE_COORD_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
    m_texture->Unbind();
  }
}
