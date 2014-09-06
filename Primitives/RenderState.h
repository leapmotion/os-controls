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

  // float AlphaMask() const { return m_alpha_mask; }
  // void SetAlphaMask(float alpha_mask) { m_alpha_mask = std::min(std::max(alpha_mask, 0.0f), 1.0f); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  // modelview and projection
  ModelView m_ModelView;
  Projection m_Projection;

  // float m_alpha_mask;
};
