#include "RadialMenu.h"

RadialMenuItem::RadialMenuItem() :
  m_Callback(nullptr),
  m_Wedge(std::make_shared<PartialDiskWithTriangle>()),
  m_Goal(std::make_shared<PartialDiskWithTriangle>()),
  m_Activation(0.0, 0.35f),
  m_Cooldown(false)
{
}

void RadialMenuItem::InitChildren() {
  AddChild(m_Wedge);
  AddChild(m_Goal);
}

void RadialMenuItem::SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon) {
  m_Icon = svgIcon;

  const EigenTypes::Vector2& origin = m_Icon->Origin();
  const EigenTypes::Vector2& size = m_Icon->Size();

  static const double ICON_THICKNESS_RATIO = 0.6;

  m_IconScale = ICON_THICKNESS_RATIO * m_Thickness / size.norm();
  m_Icon->LinearTransformation() = EigenTypes::Vector3(m_IconScale, -m_IconScale, m_IconScale).asDiagonal() * m_Icon->LinearTransformation();

  const EigenTypes::Vector2 center = origin + size/2.0;
  const EigenTypes::Vector3 iconOffset;
  m_IconOffset = m_IconScale * EigenTypes::Vector3(-center.x(), center.y(), 0);

  AddChild(m_Icon);
}

bool RadialMenuItem::Hit(const EigenTypes::Vector2& pos, double& ratio) const {
  const EigenTypes::Vector2 radial = toRadialCoordinates(pos);
  const double itemRadius = CurrentRadius();
  const double minRadius = itemRadius - m_Thickness / 2.0;
  const double maxRadius = std::numeric_limits<double>::max();//itemRadius + m_Thickness / 2.0;
  const double radius = radial[0];
  const double angle = radial[1];
  
  const bool angleOK = angle >= m_Wedge->StartAngle() && angle <= m_Wedge->EndAngle();
  const bool radiusOK = radius >= minRadius && radius <= maxRadius;
  if (angleOK && radiusOK) {
    ratio = 2 * (radius - minRadius) / m_Thickness;
    return true;
  }
  return false;
}

void RadialMenuItem::DrawContents(RenderState& renderState) const {
  const double radius = CurrentRadius();
  const double innerRadius = radius - m_Thickness/2.0;
  const double outerRadius = radius + m_Thickness/2.0;
  const double triangleRatio = 0.25;
  const double goalAngleRatio = 0.5;

  m_Wedge->SetStartAngle(m_StartAngle);
  m_Wedge->SetEndAngle(m_EndAngle);
  m_Wedge->SetInnerRadius(innerRadius);
  m_Wedge->SetOuterRadius(outerRadius);
  const Leap::GL::Rgba<float> wedgeColor = calculateColor();
  m_Wedge->Material().Uniform<AMBIENT_LIGHT_COLOR>() = wedgeColor;
  m_Wedge->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  m_Wedge->SetTriangleOffset(m_Activation > 0.001 ? triangleRatio : 0.0);
  m_Wedge->SetTriangleWidth(0.1);
  m_Wedge->SetTriangleSide(PartialDiskWithTriangle::OUTSIDE);
  
  const double goalThickness = m_Thickness/3.0;
  const double sweepAngle = (m_EndAngle - m_StartAngle);
  const double goalSweepAngle = goalAngleRatio * sweepAngle;
  const double halfWayAngle = (m_StartAngle + m_EndAngle) / 2.0;

  m_Goal->SetStartAngle(halfWayAngle - goalSweepAngle/2.0);
  m_Goal->SetEndAngle(halfWayAngle + goalSweepAngle/2.0);
  m_Goal->SetInnerRadius(m_ActivatedRadius + m_Thickness/2.0);
  m_Goal->SetOuterRadius(m_ActivatedRadius + m_Thickness/2.0 + goalThickness);
  m_Goal->Material().Uniform<AMBIENT_LIGHTING_PROPORTION>() = 1.0f;
  const Leap::GL::Rgba<float> goalColor = m_Activation > 0.001 ? m_ActivatedColor : Leap::GL::Rgba<float>::Zero(); // Zero is transparent black
  m_Goal->Material().Uniform<AMBIENT_LIGHT_COLOR>() = goalColor;
  m_Goal->SetTriangleOffset(-triangleRatio * (m_Thickness/goalThickness));
  m_Goal->SetTriangleWidth(0.1 * (sweepAngle/goalSweepAngle));
  m_Goal->SetTriangleSide(PartialDiskWithTriangle::INSIDE);

  if (m_Icon) {
    const double iconPosX = radius * std::cos(halfWayAngle);
    const double iconPosY = radius * std::sin(halfWayAngle);
    m_Icon->Translation() = EigenTypes::Vector3(iconPosX, iconPosY, 0.1) + m_IconOffset;
  }
}

Leap::GL::Rgba<float> RadialMenuItem::calculateColor() const {
  if (m_Activation < 0.001) {
    return Material().Uniform<DIFFUSE_LIGHT_COLOR>();
  }
  Leap::GL::ColorComponent<float> param(static_cast<float>(m_Activation));
  param.Clamp(); // Clamps the parameter to within the range [0,1].
  return m_HoverColor.BlendedWith(m_ActivatedColor, param);
}

double RadialMenuItem::CurrentRadius() const {
  return (1.0-m_Activation) * m_Radius + m_Activation * m_ActivatedRadius;
}

void RadialMenuItem::CheckFireCallback() {
  if (m_Cooldown) {
    if (m_Activation < 0.01) {
      m_Cooldown = false;
    }
  } else {
    if (m_Activation > 0.99) {
      if (m_Callback) {
        m_Callback->OnActivated();
      }
      m_Cooldown = true;
    }
  }
}


RadialMenu::RadialMenu() {

}

void RadialMenu::SetNumItems(int num) {
  const int prevNumItems = static_cast<int>(m_Items.size());
  if (prevNumItems == num) {
    return;
  }

  m_Items.resize(num);
  for (int i=prevNumItems; i<num; i++) {
    m_Items[i] = std::shared_ptr<RadialMenuItem>(new RadialMenuItem);
    AddChild(m_Items[i]);
    m_Items[i]->InitChildren();
  }

  updateItemLayout();
}

RadialMenu::UpdateResult RadialMenu::InteractWithCursor(const EigenTypes::Vector3& cursor) {
  const HitResult hitResult = ItemFromPoint(cursor.head<2>());
  const int numItems = static_cast<int>(m_Items.size());
  const int& idx = hitResult.hitIdx;
  const double& ratio = hitResult.hitRatio;

  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_Items[i];
    if (i != idx) {
      item->SetActivation(0.0);
    } else {
      item->SetActivation(ratio > 1.0 ? 1.0 : ratio);
    }
  }
  return UpdateResult(idx, idx >= 0 ? m_Items[idx]->CurrentActivation() : 0.0);
}

void RadialMenu::InteractWithoutCursor() {
  const int numItems = static_cast<int>(m_Items.size());

  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_Items[i];
    item->SetActivation(0.0);
  }
}

void RadialMenu::UpdateItemActivation(float deltaTime) {
  const int numItems = static_cast<int>(m_Items.size());
  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_Items[i];
    item->UpdateActivation(deltaTime);
    item->CheckFireCallback();
  }
}

RadialMenu::HitResult RadialMenu::ItemFromPoint(const EigenTypes::Vector2& pos) const {
  int hit = -1;
  double ratio = 0;
  const int numItems = static_cast<int>(m_Items.size());
  for (int i=0; i<numItems; i++) {
    if (m_Items[i]->Hit(pos, ratio)) {
      hit = i;
      break;
    }
  }

  return HitResult(hit, ratio);
}

void RadialMenu::updateItemLayout() {
  const int numItems = static_cast<int>(m_Items.size());
  if (numItems == 0) {
    return;
  }

  const double sweepAngle = m_EndAngle - m_StartAngle;
  const double anglePerItem = sweepAngle / numItems;

  double curAngle = m_StartAngle;
  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_Items[i];
    item->SetStartAngle(curAngle);
    item->SetEndAngle(curAngle + anglePerItem);

    const double halfWayAngle = curAngle + 0.5 * anglePerItem;
    const double offsetRadius = 0.5;// item->Radius() * 0.025;
    const double offsetX = offsetRadius * std::cos(halfWayAngle);
    const double offsetY = offsetRadius * std::sin(halfWayAngle);

    item->Translation() = EigenTypes::Vector3(offsetX, offsetY, 0.0);

    curAngle += anglePerItem;
  }
}

void RadialMenu::DrawContents(RenderState& renderState) const {
  // do nothing (our children will be drawn automatically during scene graph traversal)
}
