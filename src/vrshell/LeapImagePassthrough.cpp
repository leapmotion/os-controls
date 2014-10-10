#include "stdafx.h"
#include "LeapImagePassthrough.h"
#include "Leap.h"
#include "graphics\RenderFrame.h"
#include "GLShaderMatrices.h"

LeapImagePassthrough::LeapImagePassthrough() :
m_texture(std::make_shared<GLTexture2>(GLTexture2Params(640,480,GL_LUMINANCE)))
{

  for (int i = 0; i < 640 * 480; i++) {
    m_txdatatmp[i] = i / 640;
  }

  m_rect.SetSize(EigenTypes::Vector2(640, 480));
  m_rect.SetTexture(m_texture);
  m_rect.Translation() = EigenTypes::Vector3(320, 240, 0);
}

LeapImagePassthrough::~LeapImagePassthrough()
{
}

void LeapImagePassthrough::AutoInit() {
  m_renderEngine->Add(shared_from_this());
}

void LeapImagePassthrough::AnimationUpdate(const RenderFrame& frame) {
  auto leapFrame = m_controller->frame();
  auto images = leapFrame.images();

  for (auto image : images)
  {
    int w = image.width();
    int h = image.height();

    GLTexture2PixelDataReference data(GL_LUMINANCE, GL_UNSIGNED_BYTE, m_txdatatmp, 640*480);
    m_texture->UpdateTexture(data);
  }
  
}

void LeapImagePassthrough::Render(const RenderFrame& frame) const {
  glClearColor(1.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  
  PrimitiveBase::DrawSceneGraph(m_rect, frame.renderState);
}
