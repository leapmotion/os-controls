#pragma once

#include "Interactionlayer.h"

namespace Leap {
namespace GL {

class Texture2;

} // end of namespace GL
} // end of namespace Leap

class MessageLayer : public InteractionLayer {
public:
  MessageLayer(const EigenTypes::Vector3f& initialEyePos);
  //virtual ~MessageLayer ();

  virtual void Update(TimeDelta real_time_delta) override {}
  virtual void Render(TimeDelta real_time_delta) const override;
  EventHandlerAction HandleKeyboardEvent(const SDL_KeyboardEvent &ev) override;

  void SetVisible(int index, bool value) { m_Visible[index] = value; }
  bool GetVisible(int index) const { return m_Visible[index]; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  static const int NUM_MESSAGES = 4;
  void DrawMessage(int index) const;

  mutable BufferObject m_BufferObject;

  std::shared_ptr<Leap::GL::Texture2> m_HelpTexture;
  std::shared_ptr<Leap::GL::Texture2> m_LowFPSTexture;
  std::shared_ptr<Leap::GL::Texture2> m_NoOculusTexture;
  std::shared_ptr<Leap::GL::Texture2> m_NoImagesTexture;
  
  bool m_Visible[NUM_MESSAGES];
};