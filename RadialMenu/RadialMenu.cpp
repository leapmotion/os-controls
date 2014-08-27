#include "RadialMenu.h"

RadialMenuItem::RadialMenuItem() {
  m_Wedge = std::shared_ptr<PartialDiskWithTriangle>(new PartialDiskWithTriangle());
  m_Goal = std::shared_ptr<PartialDiskWithTriangle>(new PartialDiskWithTriangle());
  m_Activation.SetGoal(0.0);
  m_Activation.SetSmoothStrength(0.35f);

  AddChild(m_Wedge);
  AddChild(m_Goal);
}

void RadialMenuItem::SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon) {
  m_Icon = svgIcon;

  const Vector2& origin = m_Icon->Origin();
  const Vector2& size = m_Icon->Size();

  static const double ICON_THICKNESS_RATIO = 0.6;

  m_IconScale = ICON_THICKNESS_RATIO * m_Thickness / size.norm();
  m_Icon->LinearTransformation() = Vector3(m_IconScale, -m_IconScale, m_IconScale).asDiagonal() * m_Icon->LinearTransformation();

  const Vector2 center = origin + size/2.0;
  const Vector3 iconOffset;
  m_IconOffset = m_IconScale * Vector3(-center.x(), center.y(), 0);

  AddChild(m_Icon);
}

bool RadialMenuItem::Hit(const Vector2& pos, double& ratio) const {
  const Vector2 radial = toRadialCoordinates(pos);
  const double itemRadius = CurrentRadius();
  const double minRadius = itemRadius - m_Thickness / 2.0;
  const double maxRadius = itemRadius + m_Thickness / 2.0;
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

void RadialMenuItem::Draw(RenderState& renderState) const {
  const double radius = CurrentRadius();
  const double innerRadius = radius - m_Thickness/2.0;
  const double outerRadius = radius + m_Thickness/2.0;
  const double triangleRatio = 0.25;
  const double goalAngleRatio = 0.5;

  m_Wedge->SetStartAngle(m_StartAngle);
  m_Wedge->SetEndAngle(m_EndAngle);
  m_Wedge->SetInnerRadius(innerRadius);
  m_Wedge->SetOuterRadius(outerRadius);
  const Color wedgeColor = calculateColor();
  m_Wedge->Material().SetDiffuseLightColor(wedgeColor);
  m_Wedge->Material().SetAmbientLightColor(wedgeColor);
  m_Wedge->Material().SetAmbientLightingProportion(1.0f);
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
  m_Goal->Material().SetAmbientLightingProportion(1.0f);
  const Color goalColor = m_Activation > 0.001 ? Material().DiffuseLightColor() : Color::Transparent();
  m_Goal->Material().SetDiffuseLightColor(goalColor);
  m_Goal->Material().SetAmbientLightColor(goalColor);
  m_Goal->SetTriangleOffset(-triangleRatio * (m_Thickness/goalThickness));
  m_Goal->SetTriangleWidth(0.1 * (sweepAngle/goalSweepAngle));
  m_Goal->SetTriangleSide(PartialDiskWithTriangle::INSIDE);

  if (m_Icon) {
    const double iconPosX = radius * std::cos(halfWayAngle);
    const double iconPosY = radius * std::sin(halfWayAngle);
    m_Icon->Translation() = Vector3(iconPosX, iconPosY, 0.1) + m_IconOffset;
  }
}

Color RadialMenuItem::calculateColor() const {
  if (m_Activation < 0.001) {
    return Material().DiffuseLightColor();
  }
  const float activationFloat = static_cast<float>(m_Activation);
  const Vector4f blended = activationFloat * m_ActivatedColor.Data() + (1.0f - activationFloat) * m_HoverColor.Data();
  return Color(blended[0], blended[1], blended[2], blended[3]);
}

double RadialMenuItem::CurrentRadius() const {
  return (1.0-m_Activation) * m_Radius + m_Activation * m_ActivatedRadius;
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
  }

  updateItemLayout();
}

void RadialMenu::UpdateItemsFromCursor(const Vector3& cursor, float deltaTime) {
  double hitRatio = 0;
  const int hitEntry = checkCollision(cursor.head<2>(), hitRatio);
  const int numItems = static_cast<int>(m_Items.size());

  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_Items[i];
    if (i != hitEntry) {
      item->SetActivation(0.0);
    } else {
      item->SetActivation(hitRatio > 1.0 ? 1.0 : hitRatio);
    }
    item->UpdateActivation(deltaTime);
  }
}

int RadialMenu::checkCollision(const Vector2& pos, double& ratio) const {
  int hit = -1;
  const int numItems = static_cast<int>(m_Items.size());
  for (int i=0; i<numItems; i++) {
    if (m_Items[i]->Hit(pos, ratio)) {
      hit = i;
      break;
    }
  }

  return hit;
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

    item->Translation() = Vector3(offsetX, offsetY, 0.0);

    curAngle += anglePerItem;
  }
}

void RadialMenu::Draw(RenderState& renderState) const {
  // do nothing (our children will be drawn automatically during scene graph traversal)
}
