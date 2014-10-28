#include "stdafx.h"
#include "LeapImagePassthrough.h"
#include "Leap.h"
#include "graphics/RenderFrame.h"
#include "graphics/RenderEngine.h"
#include "osinterface/LeapInput.h"
#include "osinterface/RenderWindow.h"
#include "GLShaderMatrices.h"

LeapImagePassthrough::LeapImagePassthrough() :
m_passthroughShader(Resource<GLShader>("passthrough"))
{
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
    GLTexture2Params imageParams(width, height, GL_LUMINANCE);
    imageParams.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    imageParams.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    imageParams.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    m_texture[0] = std::make_shared<GLTexture2>(imageParams);
    m_texture[1] = std::make_shared<GLTexture2>(imageParams);

    m_rect[0].SetTexture(m_texture[0]);
    m_rect[1].SetTexture(m_texture[1]);
    m_rect[0].Material().SetUseTexture(true);
    m_rect[1].Material().SetUseTexture(true);

    GLTexture2Params distortionParams(64, 64, GL_RG32F);
    //distortionParams.SetTexParameteri(GL_GENERATE_MIPMAP, GL_TRUE);
    distortionParams.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    distortionParams.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    distortionParams.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    distortionParams.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    m_distortion[0] = std::make_shared<GLTexture2>(distortionParams);
    m_distortion[1] = std::make_shared<GLTexture2>(distortionParams);
  }
 
  for (int i = 0; i < 2; i++) {
    m_texture[i]->Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, images[i].width(), images[i].height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, images[i].data());
    m_texture[i]->Unbind();
    
    m_distortion[i]->Bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 64, GL_RG, GL_FLOAT, images[i].distortion());
    m_distortion[i]->Unbind();
  }

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
