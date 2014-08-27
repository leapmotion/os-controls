#pragma once

#include "Primitives.h"
#include "SVGPrimitive.h"
#include "Color.h"
#include "Animation.h"
#include "RadialItem.h"

class RadialSlider : public RadialItem {
public:

  RadialSlider();
  void SetMinValue(double min) { m_MinValue = min; }
  void SetMaxValue(double max) { m_MaxValue = max; }
  void SetValue(double value) { m_Value = value; }

  void SetFillColor(const Color& color) { m_FillColor = color; }
  void SetHandleColor(const Color& color) { m_HandleColor = color; }
  void SetHandleOutlineColor(const Color& color) { m_HandleOutlineColor = color; }

  void SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon);


  virtual void Draw(RenderState& renderState) const override;

protected:
  double calculateValueRatio() const;
  double calculateValueAngle() const;
  Vector3 calculateHandlePosition() const;

  double m_MinValue;
  double m_MaxValue;
  double m_Value;
  Color m_FillColor;
  Color m_HandleColor;
  Color m_HandleOutlineColor;

  mutable std::shared_ptr<PartialDisk> m_Track;
  mutable std::shared_ptr<PartialDisk> m_Fill;
  mutable std::shared_ptr<Disk> m_Handle;
  mutable std::shared_ptr<Disk> m_HandleOutline;

  double m_IconScale;
  Vector3 m_IconOffset;
  std::shared_ptr<SVGPrimitive> m_Icon;
};
