#pragma once

#include "Primitives.h"
#include "RenderableEventHandler.h"
#include "RenderState.h"
#include "LeapListener.h"
#include "RadialMenu.h"
#include "RadialSlider.h"
#include "HandCursor.h"

#include <memory>

namespace Leap {
namespace GL {

class Shader;
class GLTexture2;
class PerspectiveCamera;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.

class MediaControlLayer : public RenderableEventHandler {
public:

  MediaControlLayer();
  virtual ~MediaControlLayer();

  virtual void Update(TimeDelta real_time_delta) override;
  virtual void Render(TimeDelta real_time_delta) const override;

  void SetDimensions(int width, int height) { m_Width = width; m_Height = height; }

private:

  int m_Width;
  int m_Height;

  RadialSlider m_VolumeSlider;
  RadialMenu m_PlaybackMenu;

  std::vector<HandCursor, Eigen::aligned_allocator<HandCursor>> m_HandCursors;
  Sphere m_Cursor;

  double m_Volume;

  std::shared_ptr<PerspectiveCamera> m_Camera;
  mutable RenderState m_Renderer;

  LeapListener m_Listener;
  Leap::Controller m_Controller;

  std::shared_ptr<Shader> m_shader;
  TimePoint m_time;
};
