#pragma once
#include "graphics/RenderEngineNode.h"

#include "Primitives.h"
#include "Color.h"

#include "Resource.h"
#include "GLShader.h"
#include "GraphicsConfigs.h"

#include <string>

class RadialButton :
  public RenderEngineNode
{
public:
  RadialButton(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge = false);
  ~RadialButton();
  
  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;
  
  void Nudge(float offset);
  void SetMaxOpacity(float opacity);
  void SetOpacity(float opacity);
  const float GetOpacity() const;
  
  Vector2 GetCenterOfMass() const;
  
  template<class V>
  float DistanceToCenter(V point) { return (point - GetCenterOfMass()).norm(); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  PartialDisk m_partialDisk;
  std::string m_label;
  bool m_isNullWedge;
  
  Color m_color;
  //float m_color_r;
  //float m_color_g;
  //float m_color_b;
  float m_alpha;
  
  float m_innerRadius;
  float m_outerRadius;
  float m_maxOpacity;
};
