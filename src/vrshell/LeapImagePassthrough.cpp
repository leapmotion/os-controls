#include "stdafx.h"
#include "LeapImagePassthrough.h"
#include "Leap.h"
#include "graphics\RenderFrame.h"
#include "GLShaderMatrices.h"
#include "GLTexture2Loader.h"
#include "FreeImage.h"

LeapImagePassthrough::LeapImagePassthrough() {
  FreeImage_Initialise();

  for (int i = 0; i < 640 * 480; i++) {
    m_txdatatmp[i] = i % 255;
  }
  auto picture = Resource<GLTexture2>("public_domain_astronomy_1.jpg");
  
  GLTexture2Params params;
  params.SetWidth(256);
  params.SetHeight(256);
  params.SetInternalFormat(GL_R8);
  params.SetTarget(GL_TEXTURE_2D);
  params.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
  params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  GLTexture2PixelDataReference data(GL_RED, GL_UNSIGNED_BYTE, m_txdatatmp, 640 * 480);
  m_texture = std::make_shared<GLTexture2>(params, data);
  
  m_rect.SetSize(EigenTypes::Vector2(600, 400));
  m_rect.SetTexture(m_texture);
  m_rect.Translation() = EigenTypes::Vector3(320, 240, 0);
  m_rect.Material().SetDiffuseLightColor(Color::Blue());
  m_rect.Material().SetAmbientLightColor(Color::Green());
  m_rect.Material().SetAmbientLightingProportion(1.0f);
  m_rect.Material().SetUseTexture(true);
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
  /*
  for (auto image : images)
  {
    int w = image.width();
    int h = image.height();

    GLTexture2PixelDataReference data(GL_LUMINANCE, GL_UNSIGNED_BYTE, m_txdatatmp, 640*480);
    m_texture->UpdateTexture(data);
  }
  */
}

void LeapImagePassthrough::Render(const RenderFrame& frame) const {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glClearColor(1.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  auto shader = Resource<GLShader>("material");
  
  glEnable(GL_TEXTURE_2D);
  
  m_texture->Bind();
  PrimitiveBase::DrawSceneGraph(m_rect, frame.renderState);
}
