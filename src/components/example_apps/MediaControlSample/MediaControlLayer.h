#pragma once

#include "Primitives.h"
#include "RenderableEventHandler.h"
#include "RenderState.h"
#include "LeapListener.h"
#include "RadialMenu.h"
#include "RadialSlider.h"
#include "HandCursor.h"

#include <memory>

class GLShader;
class GLTexture2;

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

  mutable RenderState m_Renderer;

  LeapListener m_Listener;
  Leap::Controller m_Controller;

  std::shared_ptr<GLShader> m_shader;
  TimePoint m_time;
};
