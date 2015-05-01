#include "RadialSlider.h"

RadialSlider::RadialSlider() {
  m_MinValue = 0.0;
  m_MaxValue = 1.0;
  m_Value = 0.0;
  m_Callback = nullptr;

  m_Track = std::shared_ptr<PartialDisk>(new PartialDisk());
  m_Fill = std::shared_ptr<PartialDisk>(new PartialDisk());
  m_Handle = std::shared_ptr<Disk>(new Disk());
  m_HandleOutline = std::shared_ptr<Disk>(new Disk());
}

void RadialSlider::InitChildren() {
  AddChild(m_Track);
  AddChild(m_Fill);
  AddChild(m_Handle);
  AddChild(m_HandleOutline);
}

void RadialSlider::SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon) {
  m_Icon = svgIcon;

  const EigenTypes::Vector2& origin = m_Icon->Origin();
  const EigenTypes::Vector2& size = m_Icon->Size();

  static const double ICON_RADIUS_RATIO = 0.5;

  m_IconScale = ICON_RADIUS_RATIO * m_Radius / size.norm();
  m_Icon->LinearTransformation() = EigenTypes::Vector3(m_IconScale, -m_IconScale, m_IconScale).asDiagonal() * m_Icon->LinearTransformation();

  const EigenTypes::Vector2 center = origin + size/2.0;
  const EigenTypes::Vector3 iconOffset;
  m_IconOffset = m_IconScale * EigenTypes::Vector3(-center.x(), center.y(), 0);

  AddChild(m_Icon);
}

void RadialSlider::DrawContents(RenderState& renderState) const {
  const double innerRad = m_Radius - m_Thickness/2.0;
  const double outerRad = m_Radius + m_Thickness/2.0;

  m_Track->Material().Uniform<AMBIENT_LIGHT_COLOR>() = Material().Uniform<DIFFUSE_LIGHT_COLOR>();
  m_Track->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_Track->SetStartAngle(m_StartAngle);
  m_Track->SetEndAngle(m_EndAngle);
  m_Track->SetInnerRadius(innerRad);
  m_Track->SetOuterRadius(outerRad);

  m_Fill->Material().Uniform<AMBIENT_LIGHT_COLOR>() = m_FillColor;
  m_Fill->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_Fill->SetStartAngle(m_StartAngle);
  m_Fill->SetEndAngle(calculateValueAngle());
  m_Fill->SetInnerRadius(innerRad);
  m_Fill->SetOuterRadius(outerRad);
  m_Fill->Translation() = 0.1*EigenTypes::Vector3::UnitZ();

  const EigenTypes::Vector3 handlePosition = calculateHandlePosition();

  m_Handle->Material().Uniform<AMBIENT_LIGHT_COLOR>() = m_HandleColor;
  m_Handle->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_Handle->SetRadius(m_Thickness * 1.2);
  m_Handle->Translation() = handlePosition + 0.3*EigenTypes::Vector3::UnitZ();

  m_HandleOutline->Material().Uniform<AMBIENT_LIGHT_COLOR>() = m_HandleOutlineColor;
  m_HandleOutline->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_HandleOutline->SetRadius(m_Thickness * 1.33);
  m_HandleOutline->Translation() = handlePosition + 0.2*EigenTypes::Vector3::UnitZ();

  if (m_Icon) {
    m_Icon->Translation() = m_IconOffset;
  }
}

double RadialSlider::calculateValueRatio() const {
  assert(m_MaxValue > m_MinValue);
  assert(m_Value >= m_MinValue && m_Value <= m_MaxValue);
  return (m_Value - m_MinValue) / (m_MaxValue - m_MinValue);
}

double RadialSlider::calculateValueAngle() const {
  const double ratio = calculateValueRatio();
  return (1.0 - ratio) * (m_EndAngle - m_StartAngle) + m_StartAngle;
}

EigenTypes::Vector3 RadialSlider::calculateHandlePosition() const {
  const double angle = calculateValueAngle();
  const double handlePosX = m_Radius * std::cos(angle);
  const double handlePosY = m_Radius * std::sin(angle);

  return EigenTypes::Vector3(handlePosX, handlePosY, 0.0);
}
