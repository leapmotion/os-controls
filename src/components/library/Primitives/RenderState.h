#pragma once

#include <algorithm>
#include "Leap/GL/ModelViewProjection.h"
#include <memory>

namespace Leap { namespace GL { class Camera; } }

// This class is the bridge between geometry and the OpenGL state machine.
// Its main role is to encapsulate attribute and uniform addresses along with the viewing matrix transforms.
class RenderState {
public:

  // getters for modelview and projection
  const ModelView& GetModelView() const { return m_ModelView; }
  ModelView& GetModelView() { return m_ModelView; }
  const Projection& GetProjection() const { return m_Projection; }
  Projection& GetProjection() { return m_Projection; }

  bool HasCamera () const { return bool(m_Camera); }
  const Leap::GL::Camera &Camera () const {
    if (!m_Camera)
      throw std::runtime_error("No camera specified -- use RenderState::SetCamera.");
    return *m_Camera;
  }
  Leap::GL::Camera &Camera () {
    if (!m_Camera)
      throw std::runtime_error("No camera specified -- use RenderState::SetCamera.");
    return *m_Camera;
  }
  // This will potentially free the old camera.
  void SetCamera (const std::shared_ptr<Leap::GL::Camera> &camera) { m_Camera = camera; }

private:

  ModelView m_ModelView;
  Projection m_Projection;
  std::shared_ptr<Leap::GL::Camera> m_Camera;
};
