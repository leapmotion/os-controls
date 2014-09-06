#pragma once

#include "Primitives.h"
#include "SVGPrimitive.h"
#include "Color.h"
#include "Animation.h"
#include "RadialItem.h"

class RadialSliderEvent {
public:
  virtual void OnValueChanged(float value) { }
};

class RadialSlider : public RadialItem {
public:

  RadialSlider();
  void SetMinValue(double min) { m_MinValue = min; }
  void SetMaxValue(double max) { m_MaxValue = max; }
  void SetValue(double value, bool fireCallback = true) {
    m_Value = value;
    if (fireCallback && m_Callback) {
      m_Callback->OnValueChanged(static_cast<float>(m_Value));
    }
  }

  void SetFillColor(const Color& color) { m_FillColor = color; }
  void SetHandleColor(const Color& color) { m_HandleColor = color; }
  void SetHandleOutlineColor(const Color& color) { m_HandleOutlineColor = color; }

  void SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon);

  void SetCallback(RadialSliderEvent* callback) { m_Callback = callback; }

protected:
  virtual void DrawContents(RenderState& renderState) const override;

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

  RadialSliderEvent* m_Callback;

  double m_IconScale;
  Vector3 m_IconOffset;
  std::shared_ptr<SVGPrimitive> m_Icon;
};
