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
  RadialButton(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge = false);
  ~RadialButton();
  
  void Render(const RenderFrame& frame) const override;
  
  void Nudge(float offset);
  void SetOpacity(float opacity);
  
  Vector2 GetCenterOfMass() const;
  
  template<class V>
  float DistanceToCenter(V point) { return (point - GetCenterOfMass()).norm(); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  PartialDisk m_partialDisk;
  std::string m_label;
  bool m_isNullWedge;
  
  float m_color_r;
  float m_color_g;
  float m_color_b;
  float m_color_a;
  
  float m_innerRadius;
  float m_outerRadius;
  float m_maxOpacity;
};
