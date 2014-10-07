#pragma once

#include "GLController.h"
#include "RenderableEventHandler.h"

#include <memory>

class GLShader;
class GLTexture2;

// This is one "layer" of the application -- layers being rendered back to front.
class ExampleLayer : public RenderableEventHandler {
public:

  ExampleLayer ();
  virtual ~ExampleLayer ();

  virtual void Update (TimeDelta real_time_delta) override;
  virtual void Render (TimeDelta real_time_delta) const override;

private:

  std::shared_ptr<GLShader> m_shader;
  std::shared_ptr<GLTexture2> m_texture;
  TimePoint m_time;
};
