#pragma once

#include <algorithm>
#include "ModelViewProjection.h"

// This class is the bridge between geometry and the OpenGL state machine.
// Its main role is to encapsulate attribute and uniform addresses along with the viewing matrix transforms.
class RenderState {
public:

  // getters for modelview and projection
  const ModelView& GetModelView() const { return m_ModelView; }
  ModelView& GetModelView() { return m_ModelView; }
  const Projection& GetProjection() const { return m_Projection; }
  Projection& GetProjection() { return m_Projection; }

private:

  ModelView m_ModelView;
  Projection m_Projection;
};
