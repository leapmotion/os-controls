#pragma once

#include "Primitives.h"
#include "SVGPrimitive.h"
#include "Animation.h"
#include "RadialItem.h"
#include "Leap/GL/Rgba.h"

class RadialSliderEvent {
public:
  virtual void OnValueChanged(float value) { }
};

class RadialSlider : public RadialItem {
public:

  RadialSlider();
  void InitChildren();
  void SetMinValue(double min) { m_MinValue = min; }
  void SetMaxValue(double max) { m_MaxValue = max; }
  void SetValue(double value, bool fireCallback = true) {
    m_Value = value;
    if (fireCallback && m_Callback) {
      m_Callback->OnValueChanged(static_cast<float>(m_Value));
    }
  }

  void SetFillColor(const Leap::GL::Rgba<float>& color) { m_FillColor = color; }
  void SetHandleColor(const Leap::GL::Rgba<float>& color) { m_HandleColor = color; }
  void SetHandleOutlineColor(const Leap::GL::Rgba<float>& color) { m_HandleOutlineColor = color; }

  void SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon);

  void SetCallback(RadialSliderEvent* callback) { m_Callback = callback; }

protected:
  virtual void DrawContents(RenderState& renderState) const override;

  double calculateValueRatio() const;
  double calculateValueAngle() const;
  EigenTypes::Vector3 calculateHandlePosition() const;

  double m_MinValue;
  double m_MaxValue;
  double m_Value;
  Leap::GL::Rgba<float> m_FillColor;
  Leap::GL::Rgba<float> m_HandleColor;
  Leap::GL::Rgba<float> m_HandleOutlineColor;

  mutable std::shared_ptr<PartialDisk> m_Track;
  mutable std::shared_ptr<PartialDisk> m_Fill;
  mutable std::shared_ptr<Disk> m_Handle;
  mutable std::shared_ptr<Disk> m_HandleOutline;

  RadialSliderEvent* m_Callback;

  double m_IconScale;
  EigenTypes::Vector3 m_IconOffset;
  std::shared_ptr<SVGPrimitive> m_Icon;
};
