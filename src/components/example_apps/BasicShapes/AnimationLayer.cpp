#include "AnimationLayer.h"

#include <cmath>

#include "GLShaderLoader.h"
#include "GLTexture2Loader.h"
#include "Leap/GL/BufferObject.h"
#include "Leap/GL/Texture2.h"
#include "Leap/GL/PerspectiveCamera.h"
#include "Leap/GL/Shader.h"
#include "Resource.h"

using namespace Leap::GL;

AnimationLayer::AnimationLayer()
:
m_Width(640),
m_Height(480),
m_Sphere1Translation(EigenTypes::Vector3::Zero()),
m_Sphere2Translation(EigenTypes::Vector3::Zero()),
m_Sphere3Translation(EigenTypes::Vector3::Zero()),
m_Sphere4Translation(EigenTypes::Vector3::Zero()),
m_time(0)
{
  m_shader = Resource<Shader>("material");

  const Rgba<float> color1(1.0f, 0.5f, 0.3f, 1.0f);
  m_Sphere1.SetRadius(6);
  m_Sphere1.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = color1;
  m_Sphere1.Material().Uniform<AMBIENT_LIGHT_COLOR>() = color1;
  m_Sphere1.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Sphere1Translation.SetGoal(EigenTypes::Vector3::Zero());
  m_Sphere1Translation.SetSmoothStrength(0.95f);

  const Rgba<float> color2(0.9f, 0.9f, 0.4f, 1.0f);
  m_Sphere2.SetRadius(5);
  m_Sphere2.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = color2;
  m_Sphere2.Material().Uniform<AMBIENT_LIGHT_COLOR>() = color2;
  m_Sphere2.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Sphere2Translation.SetGoal(EigenTypes::Vector3::Zero());
  m_Sphere2Translation.SetSmoothStrength(0.85f);

  const Rgba<float> color3(0.3f, 1.0f, 0.5f, 1.0f);
  m_Sphere3.SetRadius(4);
  m_Sphere3.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = color3;
  m_Sphere3.Material().Uniform<AMBIENT_LIGHT_COLOR>() = color3;
  m_Sphere3.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Sphere3Translation.SetGoal(EigenTypes::Vector3::Zero());
  m_Sphere3Translation.SetSmoothStrength(0.75f);

  const Rgba<float> color4(0.4f, 0.7f, 1.0f, 1.0f);
  m_Sphere4.SetRadius(3);
  m_Sphere4.Material().Uniform<DIFFUSE_LIGHT_COLOR>() = color4;
  m_Sphere4.Material().Uniform<AMBIENT_LIGHT_COLOR>() = color4;
  m_Sphere4.Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 0.3f;
  m_Sphere4Translation.SetGoal(EigenTypes::Vector3::Zero());
  m_Sphere4Translation.SetSmoothStrength(0.65f);
}

AnimationLayer::~AnimationLayer() {

}

void AnimationLayer::Update(TimeDelta real_time_delta) {
  m_time += real_time_delta;

  const float dart1 = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
  const float dart2 = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
  const float dart3 = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
  const float dart4 = static_cast<float>(rand())/static_cast<float>(RAND_MAX);

  if (dart1 < 0.01f) {
    m_Sphere1Translation.SetGoal(25*EigenTypes::Vector3::Random());
  }
  if (dart2 < 0.03f) {
    m_Sphere2Translation.SetGoal(25*EigenTypes::Vector3::Random());
  }
  if (dart3 < 0.06f) {
    m_Sphere3Translation.SetGoal(25*EigenTypes::Vector3::Random());
  }
  if (dart4 < 0.1f) {
    m_Sphere4Translation.SetGoal(25*EigenTypes::Vector3::Random());
  }

  m_Sphere1Translation.Update(static_cast<float>(real_time_delta));
  m_Sphere2Translation.Update(static_cast<float>(real_time_delta));
  m_Sphere3Translation.Update(static_cast<float>(real_time_delta));
  m_Sphere4Translation.Update(static_cast<float>(real_time_delta));

  m_Sphere1.Translation() = (EigenTypes::Vector3)m_Sphere1Translation;
  m_Sphere2.Translation() = (EigenTypes::Vector3)m_Sphere2Translation;
  m_Sphere3.Translation() = (EigenTypes::Vector3)m_Sphere3Translation;
  m_Sphere4.Translation() = (EigenTypes::Vector3)m_Sphere4Translation;
}

void AnimationLayer::Render(TimeDelta real_time_delta) const {
  glEnable(GL_DEPTH_TEST);

  // set renderer projection matrix
  const double fovRadians = (M_PI / 180.0) * 60;
  const double widthOverHeight = static_cast<double>(m_Width)/static_cast<double>(m_Height);
  const double nearClip = 1.0;
  const double farClip = 10000.0;
  Camera::SetPerspectiveProjectionMatrix_UsingFOVAndAspectRatio(m_Renderer.ProjectionMatrix(), fovRadians, widthOverHeight, nearClip, farClip);

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

  // draw primitives
  PrimitiveBase::DrawSceneGraph(m_Sphere1, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Sphere2, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Sphere3, m_Renderer);
  PrimitiveBase::DrawSceneGraph(m_Sphere4, m_Renderer);

  m_shader->Unbind();
}
