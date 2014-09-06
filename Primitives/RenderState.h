#pragma once

#include "ModelViewProjection.h"
#include "Color.h"


// A stack for color modification during scene graph traversal.
// This is used for applying a tint or coloring to all children of a particular node.
class ColorBlendStack {
public:

  ColorBlendStack() {
    Reset();
  }

  // getters for current color value
  const Color& CurColor() const { return m_ColorStack.back(); }
  Color& CurColor() { return m_ColorStack.back(); }

  // multiply the top of the stack by another color
  // TODO: add other color blending operations, e.g. lighten or add
  void Multiply(const Color& color) {
    const Vector4f multiplied = m_ColorStack.back().Data().cwiseProduct(color.Data());
    m_ColorStack.back() = Color(multiplied);
  }

  // clear the stack and reset it to an "identity" state
  void Reset() {
    m_ColorStack.clear();
    m_ColorStack.push_back(Color::White());
  }

  // stack manipulation functions
  void Push() { m_ColorStack.push_back(m_ColorStack.back()); }
  void Pop() { m_ColorStack.pop_back(); }

private:

  std::vector<Color, Eigen::aligned_allocator<Color> > m_ColorStack;
};


// A stack for opacity modification during scene graph traversal.
// This is used for applying an fade effect to all children of a particular node.
class OpacityStack {
public:

  OpacityStack() {
    Reset();
  }

  // getters for current opacity value
  const float& Opacity() const { return m_OpacityStack.back(); }
  float& Opacity() { return m_OpacityStack.back(); }

  // multiply the top of the stack by another opacity
  void Multiply(float opacity) {
    m_OpacityStack.back() *= opacity;
  }

  // clear the stack and reset it to an "identity" state
  void Reset() {
    m_OpacityStack.clear();
    m_OpacityStack.push_back(1.0f);
  }

  // stack manipulation functions
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
  const OpacityStack& GetOpacityState() const { return m_OpacityStack; }
  OpacityStack& GetOpacityState() { return m_OpacityStack; }

private:

  // modelview and projection
  ModelView m_ModelView;
  Projection m_Projection;

  // opacity
  OpacityStack m_OpacityStack;
};
