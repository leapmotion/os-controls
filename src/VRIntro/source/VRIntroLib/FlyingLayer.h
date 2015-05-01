#pragma once

#include "Interactionlayer.h"

namespace Leap {
namespace GL {

class Shader;

} // end of namespace GL
} // end of namespace Leap

class FlyingLayer : public InteractionLayer {
public:
  FlyingLayer(const EigenTypes::Vector3f& initialEyePos);
  //virtual ~FlyingLayer ();

  virtual void Update(TimeDelta real_time_delta) override;
  virtual void Render(TimeDelta real_time_delta) const override;
  EventHandlerAction HandleKeyboardEvent(const SDL_KeyboardEvent &ev) override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  mutable BufferObject m_PopupBufferObject;
  std::shared_ptr<Texture2> m_PopupTexture;
  std::shared_ptr<Leap::GL::Shader> m_PopupShader;
  std::shared_ptr<Leap::GL::ShaderMatrices> m_PopupShaderMatrices;

  void RenderPopup() const;

  EigenTypes::Vector3f m_GridCenter;
  //EigenTypes::Vector3f m_AveragePalm;
  EigenTypes::Vector3f m_Velocity;
  EigenTypes::Vector3f m_RotationAA;
  EigenTypes::Matrix4x4f m_GridOrientation;
  float m_LineThickness;
  int m_GridBrightness;
};
