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
  m_leap->AddPolicy(static_cast<Leap::Controller::PolicyFlag>(1 >> 15)); //POLICY_INCLUDE_ALL_FRAMES

  for (int i = 0; i < 2; i++) {
    m_rect[i].SetSize(EigenTypes::Vector2(640, 480));
    m_rect[i].Translation() = EigenTypes::Vector3(320, 240, 0);
    m_rect[i].SetShader(m_passthroughShader);
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

  for (int i = 0; i < 2; i++) {
    m_rect[i].Translation() = rectPos;
    m_rect[i].SetSize({ windowSize.width, windowSize.height });
  }
}

void LeapImagePassthrough::Render(const RenderFrame& frame) const {
  auto& texture = m_texture[frame.eyeIndex];
  auto& distortion = m_distortion[frame.eyeIndex];
  if (!texture || !distortion) { 
    return;
  }

  glEnable(GL_TEXTURE_2D);
  m_passthroughShader->Bind();
  
  glActiveTexture(GL_TEXTURE0 + 0);
  texture->Bind();
  glActiveTexture(GL_TEXTURE0 + 1);
  distortion->Bind();

  const float aspectRatio = 960.f/1140; //w/h
  const float rayscale = .27f;
  glUniform2f(m_passthroughShader->LocationOfUniform("ray_scale"), rayscale, -rayscale/aspectRatio);
  glUniform2f(m_passthroughShader->LocationOfUniform("ray_offset"), 0.5f, 0.5f);
  glUniform1i(m_passthroughShader->LocationOfUniform("texture"), 0);
  glUniform1i(m_passthroughShader->LocationOfUniform("distortion"), 1);
  glUniform1f(m_passthroughShader->LocationOfUniform("gamma"), 0.8f);
  glUniform1f(m_passthroughShader->LocationOfUniform("brightness"), 1.0f);

  PrimitiveBase::DrawSceneGraph(m_rect[frame.eyeIndex], frame.renderState);

  texture->Unbind();
  distortion->Unbind();
  m_passthroughShader->Unbind();
}
