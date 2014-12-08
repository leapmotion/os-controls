#pragma once
#include "graphics/Renderable.h"
#include "graphics/RenderEngine.h"
#include "Leap/GL/Shader.h"
#include "Leap/GL/Texture2.h"
#include "Primitives.h"

class LeapInput;
namespace Leap {
  class Controller;
};

class LeapImagePassthrough :
  public std::enable_shared_from_this<LeapImagePassthrough>,
  public Renderable
{
public:
  LeapImagePassthrough();
  virtual ~LeapImagePassthrough();

  void AutoInit();

  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;

private:
  AutoRequired<LeapInput> m_leap;
  AutoRequired<Leap::Controller> m_controller;

  Autowired<RenderEngine> m_renderEngine;
  std::shared_ptr<Texture2> m_texture[2];
  std::shared_ptr<Texture2> m_distortion[2];
  std::shared_ptr<Shader> m_passthroughShader;
  RectanglePrim m_rect[2];
};

