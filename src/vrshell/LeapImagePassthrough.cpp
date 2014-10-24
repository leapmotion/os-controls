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

  for (int i = 0; i < 640 * 480; i++) {
    m_txdatatmp[i] = i % 255;
  }

  m_rect.SetSize(EigenTypes::Vector2(640, 480));
  m_rect.Translation() = EigenTypes::Vector3(320, 240, 0);
  m_rect.Material().SetAmbientLightColor(Color::White());
  m_rect.Material().SetAmbientLightingProportion(1.0f);
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

  if (!m_texture) {
    // Generate a texture procedurally.
    GLsizei width = images[0].width() + images[1].width();
    GLsizei height = images[0].height();
    GLTexture2Params params(width, height, GL_LUMINANCE);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    m_texture = std::make_shared<GLTexture2>(params);

    m_rect.SetTexture(m_texture);
    m_rect.Material().SetUseTexture(true);
  }
 
  m_texture->Bind();
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, images[0].width(), images[0].height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, images[0].data());
  glTexSubImage2D(GL_TEXTURE_2D, 0, images[0].width(), 0, images[1].width(), images[1].height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, images[1].data());
  m_texture->Unbind();

  const auto& windowSize = frame.renderWindow->GetSize();
  m_rect.SetSize({ windowSize.width, windowSize.height });
  m_rect.Translation() = EigenTypes::Vector3(windowSize.width / 2, windowSize.height / 2, 0);
}

void LeapImagePassthrough::Render(const RenderFrame& frame) const {

  glEnable(GL_TEXTURE_2D);
  
  if( m_texture) 
    m_texture->Bind();

  PrimitiveBase::DrawSceneGraph(m_rect, frame.renderState);

  if (m_texture)
    m_texture->Unbind();
}
