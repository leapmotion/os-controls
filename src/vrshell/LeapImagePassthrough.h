#pragma once
#include "graphics/Renderable.h"
#include "GLTexture2.h"
#include "GLShader.h"
#include "graphics/RenderEngine.h"
#include "Primitives.h"
#include "osinterface/OSKeyboardEvent.h"

class LeapInput;
namespace Leap {
  class Controller;
};

class LeapImagePassthrough :
  public std::enable_shared_from_this<LeapImagePassthrough>,
  public Renderable,
  public OSKeyboardEvent
{
public:
  LeapImagePassthrough();
  virtual ~LeapImagePassthrough();

  void AutoInit();

  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;

  void KeyDown(int keycode) override;

private:
  AutoRequired<LeapInput> m_leap;
  AutoRequired<Leap::Controller> m_controller;

  Autowired<RenderEngine> m_renderEngine;
  std::shared_ptr<GLTexture2> m_texture[2];
  std::shared_ptr<GLTexture2> m_distortion[2];
  std::shared_ptr<GLShader> m_passthroughShader;
  RectanglePrim m_rect[2];

  EigenTypes::Vector2f m_fov;
  float m_aspectRatio;
};

