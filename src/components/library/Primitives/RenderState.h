#pragma once

#include <algorithm>
#include "EigenTypes.h"
#include "Leap/GL/Camera.h"
#include "Leap/GL/ModelViewProjection.h"
#include <memory>

using namespace Leap::GL;

// This class is the bridge between geometry and the OpenGL state machine.
// Its main role is to encapsulate attribute and uniform addresses along with the viewing matrix transforms.
class RenderState {
public:

  RenderState () {
    // This should be the same as the default projection matrix in old versions of OpenGL.
    Camera::SetOrthographicProjectionMatrix(m_ProjectionMatrix, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  }

  const EigenTypes::Matrix4x4 &ProjectionMatrix () const { return m_ProjectionMatrix; }
  EigenTypes::Matrix4x4 &ProjectionMatrix () { return m_ProjectionMatrix; }

  // getters for modelview and projection
  const ModelView& GetModelView() const { return m_ModelView; }
  ModelView& GetModelView() { return m_ModelView; }

private:

  EigenTypes::Matrix4x4 m_ProjectionMatrix;
  ModelView m_ModelView;
  std::shared_ptr<Leap::GL::Camera> m_Camera;
};
