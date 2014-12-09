#include "stdafx.h"
#include "MessageLayer.h"

#include "Resource.h"
#include "Leap/GL/Texture2.h"

using Leap::GL::Texture2;
#include "GLTexture2Loader.h"

using namespace Leap::GL;

MessageLayer::MessageLayer(const EigenTypes::Vector3f& initialEyePos) :
  InteractionLayer(EigenTypes::Vector3f::Zero(), "shaders/transparent"),
  m_HelpTexture(Resource<Texture2>("images/help.png")),
  m_LowFPSTexture(Resource<Texture2>("images/lowfps.png")),
  m_NoOculusTexture(Resource<Texture2>("images/no_oculus.png")),
  m_NoImagesTexture(Resource<Texture2>("images/no_images.png")) {

  static const float edges[] = {
    // No Images warning
    -0.4f, -0.3f, -0.6f, 0, 0,
    -0.4f, +0.3f, -0.6f, 0, 1,
    +0.4f, -0.3f, -0.6f, 1, 0,
    +0.4f, +0.3f, -0.6f, 1, 1,
    
    // Help menu
    -0.224f, -0.264f, -0.5f, 0, 0,
    -0.224f, +0.264f, -0.5f, 0, 1,
    +0.224f, -0.264f, -0.5f, 1, 0,
    +0.224f, +0.264f, -0.5f, 1, 1,

    // Low FPS warning
    -0.288f, -0.12f, -0.5f, 0, 0,
    -0.288f, +0.12f, -0.5f, 0, 1,
    +0.288f, -0.12f, -0.5f, 1, 0,
    +0.288f, +0.12f, -0.5f, 1, 1,

    // No Oculus warning
    -0.288f, -0.184f, -0.3f, 0, 0,
    -0.288f, +0.184f, -0.3f, 0, 1,
    +0.288f, -0.184f, -0.3f, 1, 0,
    +0.288f, +0.184f, -0.3f, 1, 1,
  };

  m_Buffer.Initialize(GL_ARRAY_BUFFER);
  m_Buffer.Bind();
  m_Buffer.Allocate(edges, sizeof(edges), GL_STATIC_DRAW);
  m_Buffer.Unbind();

  m_Visible[0] = false;
  m_Visible[1] = true;
  for (int i = 2; i < NUM_MESSAGES; i++) {
    m_Visible[i] = false;
  }
}

void MessageLayer::Render(TimeDelta real_time_delta) const {
  m_Shader->Bind();
  EigenTypes::Matrix4x4f modelView = m_ModelView;
  modelView.block<3, 1>(0, 3) += modelView.block<3, 3>(0, 0)*m_EyePos;
  modelView.block<3, 3>(0, 0) = EigenTypes::Matrix3x3f::Identity();
  m_ShaderMatrices->SetMatrices(modelView.cast<double>(), m_Renderer.Camera().ProjectionMatrix());
  m_ShaderMatrices->UploadUniforms();

  glActiveTexture(GL_TEXTURE0 + 0);
  glUniform1i(m_Shader->LocationOfUniform("texture"), 0);
  glUniform1f(m_Shader->LocationOfUniform("alpha"), 1.0f);

  m_Buffer.Bind();
  glEnableVertexAttribArray(m_Shader->LocationOfAttribute("position"));
  glEnableVertexAttribArray(m_Shader->LocationOfAttribute("texcoord"));
  glVertexAttribPointer(m_Shader->LocationOfAttribute("position"), 3, GL_FLOAT, GL_TRUE, 5*sizeof(float), (GLvoid*)0);
  glVertexAttribPointer(m_Shader->LocationOfAttribute("texcoord"), 2, GL_FLOAT, GL_TRUE, 5*sizeof(float), (GLvoid*)(3*sizeof(float)));

  int topMessage = -1;
  for (int i = NUM_MESSAGES - 1; i >= 0; i--) {
    if (m_Visible[i]) {
      topMessage = i;
    }
  }
  if (topMessage >= 0) {
    DrawMessage(topMessage);
  }

  glDisableVertexAttribArray(m_Shader->LocationOfAttribute("position"));
  glDisableVertexAttribArray(m_Shader->LocationOfAttribute("texcoord"));
  m_Buffer.Unbind();

  m_Shader->Unbind();
}

void MessageLayer::DrawMessage(int index) const {
  switch (index) {
  case 0:
    m_NoImagesTexture->Bind();
    break;
  case 1:
    m_HelpTexture->Bind();
    break;
  case 2:
    m_LowFPSTexture->Bind();
    break;
  case 3:
    m_NoOculusTexture->Bind();
    break;
  default:
    return;
  }
  glDrawArrays(GL_TRIANGLE_STRIP, 4*index, 4);
  glBindTexture(GL_TEXTURE_2D, 0); // Unbind
}

EventHandlerAction MessageLayer::HandleKeyboardEvent(const SDL_KeyboardEvent &ev) {
  //switch (ev.keysym.sym) {
  //default:
  return EventHandlerAction::PASS_ON;
  //}
}
