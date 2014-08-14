#pragma once
#include "graphics/RenderEngineNode.h"

#include "Primitives.h"
#include "Color.h"

#include "Resource.h"
#include "GLShader.h"

#include <string>

class RadialButton :
  public RenderEngineNode
{
public:
  RadialButton(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset);
  ~RadialButton();
  
  void nudge(float offset);
  void setOpacity(float opacity);
  void Render(const RenderFrame& frame) const override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  PartialDisk m_partialDisk;
  std::string m_label;
  
  float m_innerRadius;
  float m_outerRadius;
};
