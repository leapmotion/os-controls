#pragma once

#include "RenderableEventHandler.h"

#include <memory>

namespace Leap {
namespace GL {

class Shader;
class Texture2;

} // end of namespace GL
} // end of namespace Leap

// This is one "layer" of the application -- layers being rendered back to front.
class ExampleLayer : public RenderableEventHandler {
public:

  ExampleLayer ();
  virtual ~ExampleLayer ();

  virtual void Update (TimeDelta real_time_delta) override;
  virtual void Render (TimeDelta real_time_delta) const override;

private:

  std::shared_ptr<Leap::GL::Shader> m_shader;
  std::shared_ptr<Leap::GL::Texture2> m_texture;
  TimePoint m_time;
};
