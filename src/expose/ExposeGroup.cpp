#include "stdafx.h"
#include <Primitives.h>
#include "ExposeGroup.h"
#include "ExposeViewWindow.h"
#include "osinterface/OSWindow.h"
#include "osinterface/OSApp.h"
#include "graphics/RenderFrame.h"

ExposeGroup::ExposeGroup() :
  m_center(Vector2::Zero()),
  m_minBounds(Vector2::Zero()),
  m_maxBounds(Vector2::Zero()),
  m_dropShadow(new DropShadow)
{
}

void ExposeGroup::CalculateCenterAndBounds() {
  const int numGroupMembers = static_cast<int>(m_groupMembers.size());
  assert(numGroupMembers > 0);
  m_minBounds.setConstant(DBL_MAX);
  m_maxBounds.setConstant(-DBL_MAX);
  m_center.setZero();
  for (const std::shared_ptr<ExposeViewWindow>& window : m_groupMembers) {
    const Vector2 windowSize = window->GetOSSize();
    const Vector2 windowPos = window->GetOSPosition();// +0.5*windowSize;
    m_minBounds = m_minBounds.cwiseMin(windowPos - 0.5*windowSize);
    m_maxBounds = m_maxBounds.cwiseMax(windowPos + 0.5*windowSize);
    m_center += windowPos;
  }
  m_center /= numGroupMembers;
}

bool ExposeGroup::Intersects(const ExposeGroup& other) const {
  for (int d=0; d<2; d++) {
    if (other.m_maxBounds[d] < m_maxBounds[d] || other.m_minBounds[d] > m_maxBounds[d]) {
      return false;
    }
  }
  return true;
}

Vector2 ExposeGroup::MinMovementToResolveCollision(const ExposeGroup& other) const {
  const double diffX1 = other.m_maxBounds.x() - m_minBounds.x();
  const double diffX2 = m_maxBounds.x() - other.m_minBounds.x();
  const double diffY1 = other.m_maxBounds.y() - m_minBounds.y();
  const double diffY2 = m_maxBounds.y() - other.m_minBounds.y();

  double minX = diffX1;
  if (std::abs(diffX2) < std::abs(diffX1)) {
    minX = diffX2;
  }

  double minY = diffY1;
  if (std::abs(diffY2) < std::abs(diffY1)) {
    minY = diffY2;
  }

  Vector2 result(Vector2::Zero());
  if (std::abs(minX) < std::abs(minY)) {
    result.x() = minX;
  } else {
    result.y() = minY;
  }
  return result;
}

void ExposeGroup::Move(const Vector2& displacement) {
  m_center += displacement;
  m_minBounds += displacement;
  m_maxBounds += displacement;
}

void ExposeGroup::Render(const RenderFrame& frame) const {
  for (const std::shared_ptr<ExposeViewWindow>& window : m_groupMembers) {
    const float hover = window->m_hover.Value();
    if (hover > 0.01f) {
      const float tempMask = window->GetTexture()->LocalProperties().AlphaMask();
      window->GetTexture()->LocalProperties().AlphaMask() = hover;
      window->Render(frame);
      window->GetTexture()->LocalProperties().AlphaMask() = tempMask;
    }
  }

  static const Vector3 DROP_SHADOW_OFFSET(0, 0, 0);
  static const double DROP_SHADOW_RADIUS = 150.0;
  static const float DROP_SHADOW_OPACITY = 0.35f;
  m_dropShadow->Translation() = m_icon->Translation() + DROP_SHADOW_OFFSET;
  m_dropShadow->SetBasisRectangleSize(0.75*m_icon->Size());
  m_dropShadow->LinearTransformation() = m_icon->LinearTransformation();
  m_dropShadow->SetShadowRadius(DROP_SHADOW_RADIUS);
  m_dropShadow->LocalProperties().AlphaMask() = DROP_SHADOW_OPACITY * m_icon->LocalProperties().AlphaMask();
  PrimitiveBase::DrawSceneGraph(*m_dropShadow, frame.renderState);

  PrimitiveBase::DrawSceneGraph(*m_icon, frame.renderState);
}
