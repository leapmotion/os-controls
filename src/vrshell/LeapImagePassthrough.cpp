#include "stdafx.h"
#include "LeapImagePassthrough.h"
#include "Leap.h"
#include "graphics\RenderFrame.h"
#include "GLShaderMatrices.h"

LeapImagePassthrough::LeapImagePassthrough() {

  for (int i = 0; i < 640 * 480; i++) {
    m_txdatatmp[i] = i % 255;
  }

  m_rect.SetSize(EigenTypes::Vector2(640, 480));
  m_rect.Translation() = EigenTypes::Vector3(320, 240, 0);
  m_rect.Material().SetDiffuseLightColor(Color::Blue());
  m_rect.Material().SetAmbientLightColor(Color::Green());
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

  auto images = leapFrame.images();
  
  if (images.count() == 0)
    return;

  auto image = images[0];
  if (!m_texture) {
    // Generate a texture procedurally.
    GLsizei width = image.width();
    GLsizei height = image.height();
    GLTexture2Params params(width, height, GL_LUMINANCE);
    params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    GLTexture2PixelDataReference pixel_data(GL_LUMINANCE, GL_UNSIGNED_BYTE, image.data(), width*height);
    m_texture = std::make_shared<GLTexture2>(params, pixel_data);
    m_rect.SetTexture(m_texture);
    m_rect.Material().SetUseTexture(true);
  }
  else {
    GLTexture2PixelDataReference pixel_data(GL_LUMINANCE, GL_UNSIGNED_BYTE, image.data(), image.width() * image.height());
    m_texture->UpdateTexture(pixel_data);
  }
}

void LeapImagePassthrough::Render(const RenderFrame& frame) const {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glClearColor(1.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  auto shader = Resource<GLShader>("material");
  
  glEnable(GL_TEXTURE_2D);
  
  if( m_texture) 
    m_texture->Bind();

  PrimitiveBase::DrawSceneGraph(m_rect, frame.renderState);

  if (m_texture)
    m_texture->Unbind();
}
