#pragma once

#include "Primitives.h"
#include "RenderableEventHandler.h"
#include "RenderState.h"
#include "Animation.h"

#include <memory>

namespace Leap {
namespace GL {

class GLShader;
class PerspectiveCamera;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.

// This is one "layer" of the application -- layers being rendered back to front.
class AnimationLayer : public RenderableEventHandler {
public:

  AnimationLayer();
  virtual ~AnimationLayer();

  virtual void Update(TimeDelta real_time_delta) override;
  virtual void Render(TimeDelta real_time_delta) const override;

  void SetDimensions(int width, int height) { m_Width = width; m_Height = height; }

private:

  int m_Width;
  int m_Height;
  Sphere m_Sphere1;
  Sphere m_Sphere2;
  Sphere m_Sphere3;
  Sphere m_Sphere4;
  std::shared_ptr<PerspectiveCamera> m_Camera;
  mutable RenderState m_Renderer;

  Smoothed<EigenTypes::Vector3> m_Sphere1Translation;
  Smoothed<EigenTypes::Vector3> m_Sphere2Translation;
  Smoothed<EigenTypes::Vector3> m_Sphere3Translation;
  Smoothed<EigenTypes::Vector3> m_Sphere4Translation;

  std::shared_ptr<GLShader> m_shader;
  TimePoint m_time;
};
