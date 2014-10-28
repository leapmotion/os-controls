#include "stdafx.h"
#include "LeapImagePassthrough.h"
#include "Leap.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "osinterface/LeapInput.h"
#include "osinterface/RenderWindow.h"
#include "GLShaderMatrices.h"

LeapImagePassthrough::LeapImagePassthrough() {
  m_leap->AddPolicy(Leap::Controller::POLICY_IMAGES);

  for (int i = 0; i < 2; i++) {
    m_rect[i].SetSize(EigenTypes::Vector2(640, 480));
    m_rect[i].Translation() = EigenTypes::Vector3(320, 240, 0);
    m_rect[i].Material().SetAmbientLightColor(Color::White());
    m_rect[i].Material().SetAmbientLightingProportion(1.0f);
  }
}

LeapImagePassthrough::~LeapImagePassthrough()
{
}

void LeapImagePassthrough::AutoInit() {
  m_renderEngine->Add(shared_from_this());
}

void LeapImagePassthrough::AnimationUpdate(const RenderFrame& frame) {
  auto leapFrame = m_controller->frame();
  const Leap::ImageList& images = leapFrame.images();
  
  if (images.count() == 0)
    return;

  if (!m_texture[0]) {
    // Generate a texture procedurally.
    GLsizei width = images[0].width();
    GLsizei height = images[0].height();
    GLTexture2Params params(width, height, GL_LUMINANCE);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    m_texture[0] = std::make_shared<GLTexture2>(params);
    m_texture[1] = std::make_shared<GLTexture2>(params);

    m_rect[0].SetTexture(m_texture[0]);
    m_rect[1].SetTexture(m_texture[1]);
    m_rect[0].Material().SetUseTexture(true);
    m_rect[1].Material().SetUseTexture(true);
  }
 
  m_texture[0]->Bind();
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, images[0].width(), images[0].height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, images[0].data());
  m_texture[0]->Unbind();
  m_texture[1]->Bind();
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, images[1].width(), images[1].height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, images[1].data());
  m_texture[1]->Unbind();

  
  const auto& windowSize = frame.renderWindow->GetSize();
  const auto rectPos = EigenTypes::Vector3(windowSize.width / 2, windowSize.height / 2, 0);
  const double conversionFactor = windowSize.width / images[0].width();
  EigenTypes::Vector2 rectSize = { images[0].width() * conversionFactor, images[0].height() * conversionFactor };

  for (int i = 0; i < 2; i++) {
    m_rect[i].Translation() = rectPos;
    m_rect[i].SetSize(rectSize);
  }
}

void LeapImagePassthrough::Render(const RenderFrame& frame) const {

  glEnable(GL_TEXTURE_2D);
  
  auto& texture = m_texture[frame.eyeIndex];
  if (texture)
    texture->Bind();

  PrimitiveBase::DrawSceneGraph(m_rect[frame.eyeIndex], frame.renderState);

  if (texture)
    texture->Unbind();
}
