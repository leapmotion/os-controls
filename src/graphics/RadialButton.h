#pragma once
#include "State.h"
#include "graphics/RenderFrame.h"

#include "Primitives.h"
#include "Color.h"

#include "Resource.h"
#include "GLShader.h"

#include <string>

class RadialButton
{
public:
  RadialButton(const Vector3& center, float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset);
  ~RadialButton();
  
  void nudge(float offset);
  
  void draw(const RenderFrame& frame);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  PartialDisk m_partialDisk;
  std::string m_label;
  
  float m_innerRadius;
  float m_outerRadius;
};
