#pragma once

#include "ModelViewProjection.h"
#include "Color.h"

class ColorBlendState {
public:
  ColorBlendState() {
    Reset();
  }
  const Color& CurColor() const { return m_ColorStack.back(); }
  void Multiply(const Color& color) {
    const Vector4f multiplied = m_ColorStack.back().Data().cwiseProduct(color.Data());
    m_ColorStack.back() = Color(multiplied);
  }
  void Brighten(const Color& color) {

  }
  void Screen(const Color& color) {

  }
  void Reset() {
    m_ColorStack.clear();
    m_ColorStack.push_back(Color::White());
  }
  void Push() { m_ColorStack.push_back(m_ColorStack.back()); }
  void Pop() { m_ColorStack.pop_back(); }
private:
  std::vector<Color, Eigen::aligned_allocator<Color> > m_ColorStack;
};


class OpacityState {
public:
  OpacityState() {
    Reset();
  }
  const float& Opacity() const { return m_OpacityStack.back(); }
  float& Opacity() { return m_OpacityStack.back(); }
  void Multiply(float opacity) {
    m_OpacityStack.back() *= opacity;
  }
  void Reset() {
    m_OpacityStack.clear();
    m_OpacityStack.push_back(1.0f);
  }
  void Push() { m_OpacityStack.push_back(m_OpacityStack.back()); }
  void Pop() { m_OpacityStack.pop_back(); }
private:
  std::vector<float> m_OpacityStack;
};

// This class is the bridge between geometry and the OpenGL state machine.
// Its main role is to encapsulate attribute and uniform addresses along with the viewing matrix transforms.
class RenderState {
public:

  // getters for modelview and projection
  const ModelView& GetModelView() const { return m_ModelView; }
  ModelView& GetModelView() { return m_ModelView; }
  const Projection& GetProjection() const { return m_Projection; }
  Projection& GetProjection() { return m_Projection; }
  const OpacityState& GetOpacityState() const { return m_OpacityState; }
  OpacityState& GetOpacityState() { return m_OpacityState; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  // modelview and projection
  ModelView m_ModelView;
  Projection m_Projection;
  OpacityState m_OpacityState;
};
