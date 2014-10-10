#pragma once
#include "graphics\Renderable.h"
#include "GLTexture2.h"
#include "PrimitiveGeometry.h"
#include "GLShader.h"
#include "graphics\RenderEngine.h"
#include "Primitives.h"

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
  AutoRequired<Leap::Controller> m_controller;
  Autowired<RenderEngine> m_renderEngine;
  std::shared_ptr<GLTexture2> m_texture;
  RectanglePrim m_rect;
  char m_txdatatmp[640 * 480];
};

