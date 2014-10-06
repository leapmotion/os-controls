#pragma once
#include "EigenTypes.h"
#include "graphics/Renderable.h"
#include "DropShadow.h"

class ExposeViewWindow;
class OSApp;
class ImagePrimitive;

class ExposeGroup : public Renderable {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  ExposeGroup();
  void CalculateCenterAndBounds();
  bool Intersects(const ExposeGroup& other) const;
  EigenTypes::Vector2 MinMovementToResolveCollision(const ExposeGroup& other) const;
  void Move(const EigenTypes::Vector2& displacement);

  // RenderEngineNode overrides
  void Render(const RenderFrame& frame) const override;
  bool IsVisible() const override { return m_icon->LocalProperties().AlphaMask() > 0.001f; }

  EigenTypes::Vector2 m_minBounds;
  EigenTypes::Vector2 m_maxBounds;
  EigenTypes::Vector2 m_center;
  std::shared_ptr<OSApp> m_app;
  std::shared_ptr<ImagePrimitive> m_icon;
  std::shared_ptr<DropShadow> m_dropShadow;
  std::unordered_set<std::shared_ptr<ExposeViewWindow>> m_groupMembers;
};
