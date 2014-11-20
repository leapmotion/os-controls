#include "MediaControlLayer.h"

#include <cmath>

#include "GLShaderLoader.h"
#include "GLTexture2Loader.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLTexture2.h"
#include "Resource.h"

Color bgColor(0.4f, 0.425f, 0.45f, 0.75f);
Color fillColor(0.4f, 0.8f, 0.4f, 0.7f);
Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
Color handleOutlineColor(0.6f, 1.0f, 0.6f, 1.0f);

const double innerTrackRad = 8.25;
const double outerTrackRad = 9;
const double innerRad = 10;
const double outerRad = 17;

const double PI_2 = M_PI/2.0;
const double PI_4 = M_PI/4.0;

MediaControlLayer::MediaControlLayer() :
  m_Width(640),
  m_Height(480),
  m_time(0)
{
  m_shader = Resource<GLShader>("material");

  m_RewindTexture = Resource<GLTexture2>("rewind.png");
  m_PlayPauseTexture = Resource<GLTexture2>("playpause.png");
  m_FastForwardTexture = Resource<GLTexture2>("fastforward.png");

  m_LeftSection.SetStartAngle(PI_2 + PI_4);
  m_LeftSection.SetEndAngle(2*PI_2 + PI_4);
  m_LeftSection.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = bgColor;
  m_LeftSection.Material().Uniform<AMBIENT_LIGHT_COLOR>() = bgColor;
  m_LeftSection.SetInnerRadius(innerRad);
  m_LeftSection.SetOuterRadius(outerRad);
  m_LeftSection.Translation() = EigenTypes::Vector3(-0.25, 0, 0);

  m_TopSection.SetStartAngle(PI_4);
  m_TopSection.SetEndAngle(PI_2 + PI_4);
  m_TopSection.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = bgColor;
  m_TopSection.Material().Uniform<AMBIENT_LIGHT_COLOR>() = bgColor;
  m_TopSection.SetInnerRadius(innerRad);
  m_TopSection.SetOuterRadius(outerRad);
  m_TopSection.Translation() = EigenTypes::Vector3(0, 0.25, 0);

  m_RightSection.SetStartAngle(-PI_4);
  m_RightSection.SetEndAngle(PI_4);
  m_RightSection.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = bgColor;
  m_RightSection.Material().Uniform<AMBIENT_LIGHT_COLOR>() = bgColor;
  m_RightSection.SetInnerRadius(innerRad);
  m_RightSection.SetOuterRadius(outerRad);
  m_RightSection.Translation() = EigenTypes::Vector3(0.25, 0, 0);

  m_VolumeFill.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = fillColor;
  m_VolumeFill.Material().Uniform<AMBIENT_LIGHT_COLOR>() = fillColor;

  m_VolumeTrack.SetStartAngle(-PI_4);
  m_VolumeTrack.SetEndAngle(2*PI_2 + PI_4);
  m_VolumeTrack.SetInnerRadius(innerTrackRad);
  m_VolumeTrack.SetOuterRadius(outerTrackRad);
  m_VolumeTrack.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = bgColor;
  m_VolumeTrack.Material().Uniform<AMBIENT_LIGHT_COLOR>() = bgColor;

  m_VolumeFill.SetStartAngle(-PI_4);
  m_VolumeFill.SetEndAngle(2*PI_2 + PI_4);
  m_VolumeFill.SetInnerRadius(innerTrackRad);
  m_VolumeFill.SetOuterRadius(outerTrackRad);
  m_VolumeFill.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = fillColor;
  m_VolumeFill.Material().Uniform<AMBIENT_LIGHT_COLOR>() = fillColor;

  m_VolumeHandle.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = handleColor;
  m_VolumeHandle.Material().Uniform<AMBIENT_LIGHT_COLOR>() = handleColor;
  m_VolumeHandle.SetRadius(0.9);

  m_VolumeHandleOutline.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = handleOutlineColor;
  m_VolumeHandleOutline.Material().Uniform<AMBIENT_LIGHT_COLOR>() = handleOutlineColor;
  m_VolumeHandleOutline.SetRadius(1.0);

  m_Rewind.Translation() = EigenTypes::Vector3(-(innerRad+outerRad)/2.0, 0.0, 0.1);
  m_Rewind.SetSize(EigenTypes::Vector2(2, 2));
  m_Rewind.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Color::White();
  m_Rewind.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Color::White();

  m_PlayPause.Translation() = EigenTypes::Vector3(0.0, (innerRad+outerRad)/2.0, 0.1);
  m_PlayPause.SetSize(EigenTypes::Vector2(2, 2));
  m_PlayPause.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Color::White();
  m_PlayPause.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Color::White();

  m_FastForward.Translation() = EigenTypes::Vector3((innerRad+outerRad)/2.0, 0.0, 0.1);
  m_FastForward.SetSize(EigenTypes::Vector2(2, 2));
  m_FastForward.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = Color::White();
  m_FastForward.Material().Uniform<AMBIENT_LIGHT_COLOR>() = Color::White();
}

MediaControlLayer::~MediaControlLayer() {

}

void MediaControlLayer::Update(TimeDelta real_time_delta) {
  m_time += real_time_delta;

  m_Volume = 0.5 * (1.0 + std::sin(0.5*m_time));

  const double volumeAngle = m_Volume * 3 * PI_2 - PI_4;

  m_VolumeFill.SetStartAngle(volumeAngle);

  const double handleRad = (innerTrackRad + outerTrackRad)/2.0;
  const double handlePosX = handleRad*std::cos(volumeAngle);
  const double handlePosY = handleRad*std::sin(volumeAngle);

  m_VolumeHandle.Translation() = EigenTypes::Vector3(handlePosX, handlePosY, 0.2);
  m_VolumeHandleOutline.Translation() = EigenTypes::Vector3(handlePosX, handlePosY, 0.1);
}

void MediaControlLayer::Render(TimeDelta real_time_delta) const {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // set renderer projection matrix
  const double fovRadians = (M_PI / 180.0) * 60;
  const double widthOverHeight = static_cast<double>(m_Width)/static_cast<double>(m_Height);
  const double nearClip = 1.0;
  const double farClip = 10000.0;
  m_Renderer.GetProjection().Perspective(fovRadians, widthOverHeight, nearClip, farClip);

  // set renderer modelview matrix
  const EigenTypes::Vector3 eyePos = 100*EigenTypes::Vector3::UnitZ();
  const EigenTypes::Vector3 lookAtPoint = EigenTypes::Vector3::Zero();
  const EigenTypes::Vector3 upVector = EigenTypes::Vector3::UnitY();
  //m_Renderer.SetShader(m_shader);
  m_Renderer.GetModelView().Reset();
  m_Renderer.GetModelView().LookAt(eyePos, lookAtPoint, upVector);

  m_shader->Bind();

  // set light position
  const EigenTypes::Vector3f desiredLightPos(0, 10, 10);
  const EigenTypes::Vector3f lightPos = desiredLightPos - eyePos.cast<float>();
  m_shader->UploadUniform<GL_FLOAT_VEC3>("light_position", lightPos);
  m_shader->UploadUniform<GL_BOOL>("use_texture", false);

  // draw primitives
  PrimitiveBase::DrawSceneGraph(m_LeftSection, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_TopSection, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_RightSection, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_VolumeFill, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_VolumeTrack, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_VolumeHandle, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_VolumeHandleOutline, m_Renderer);

  glEnable(GL_TEXTURE_2D);
  m_shader->Bind();
  m_shader->UploadUniform<GL_BOOL>("use_texture", true);
  m_shader->UploadUniform<GL_SAMPLER_2D>("texture", 0);

  m_RewindTexture->Bind();
  PrimitiveBase::DrawSceneGraph(m_Rewind, m_Renderer);
  m_RewindTexture->Unbind();

  m_PlayPauseTexture->Bind();
  PrimitiveBase::DrawSceneGraph(m_PlayPause, m_Renderer);
  m_PlayPauseTexture->Unbind();

  m_FastForwardTexture->Bind();
  PrimitiveBase::DrawSceneGraph(m_FastForward, m_Renderer);
  m_FastForwardTexture->Unbind();

  glDisable(GL_TEXTURE_2D);

  m_shader->Unbind();
}
