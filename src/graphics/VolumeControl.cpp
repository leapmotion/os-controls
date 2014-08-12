#include "stdafx.h"
#include "VolumeControl.h"
#include "GLShaderLoader.h"

VolumeControl::VolumeControl() :
m_time(0)
{
  m_shader = Resource<GLShader>("default");

  m_partialDisk.FullTransform().block<3,1>(0,3) = Vector3(30, -20, 0);
  m_partialDisk.SetDiffuseColor(Color(1.0f, 0.3f, 1.0f, 1.0f));
  m_partialDisk.SetAmbientFactor(0.9f);

  // set renderer projection matrix
  m_renderState.GetProjection().Orthographic(0, 0, 800, 600, 0, 100);

  // set renderer modelview matrix
  const Vector3 eyePos = 100 * Vector3::UnitZ();
  const Vector3 lookAtPoint(400, 300, 0);
  const Vector3 upVector = Vector3::UnitY();
  m_renderState.GetModelView().Reset();
  m_renderState.GetModelView().LookAt(eyePos, lookAtPoint, upVector);
   
  // retrieve attribute/uniform locations from shader
  m_renderState.SetPositionAttribute(m_shader->LocationOfAttribute("position"));
  m_renderState.SetNormalAttribute(m_shader->LocationOfAttribute("normal"));
  m_renderState.SetModelViewMatrixUniform(m_shader->LocationOfUniform("modelView"));
  m_renderState.SetProjectionMatrixUniform(m_shader->LocationOfUniform("projection"));
  m_renderState.SetNormalMatrixUniform(m_shader->LocationOfUniform("normalMatrix"));
  m_renderState.SetDiffuseColorUniform(m_shader->LocationOfUniform("diffuseColor"));
  m_renderState.SetAmbientFactorUniform(m_shader->LocationOfUniform("ambientFactor"));
}

VolumeControl::~VolumeControl()
{
}

void VolumeControl::AutoFilter(const RenderFrame& frame, const StateSentry<State::VolumeControl>& in) {
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  const auto size = frame.renderWindow->getSize();

  const double fovRadians = (M_PI / 180.0) * 60;
  const double widthOverHeight = static_cast<double>(size.x) / static_cast<double>(size.y);
  const double nearClip = 1.0;
  const double farClip = 10000.0;
  m_renderState.GetProjection().Perspective(fovRadians, widthOverHeight, nearClip, farClip);

  // set renderer modelview matrix
  const Vector3 eyePos = 100 * Vector3::UnitZ();
  const Vector3 lookAtPoint = Vector3::Zero();
  const Vector3 upVector = Vector3::UnitY();
  m_renderState.GetModelView().Reset();
  m_renderState.GetModelView().LookAt(eyePos, lookAtPoint, upVector);

  m_time += frame.deltaT.count(); 

  m_partialDisk.SetStartAngle(M_PI * (1 + std::sin(0.5*m_time)));
  m_partialDisk.SetEndAngle(2 * M_PI * (1 + std::sin(0.5*m_time)));
  m_partialDisk.SetInnerRadius(4 + std::cos(m_time));
  m_partialDisk.SetOuterRadius(8 + std::cos(0.66*m_time));

  m_shader->Bind();

  // set light position
  const Vector3f desiredLightPos(0, 10, 10);
  const Vector3f lightPos = desiredLightPos - eyePos.cast<float>();
  const int lightPosLoc = m_shader->LocationOfUniform("lightPosition");
  m_shader->SetUniformf("lightPosition", lightPos);

  // draw primitives
  m_partialDisk.DrawScene(m_renderState);

  m_shader->Unbind();
}