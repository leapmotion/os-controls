#pragma once
#include "EigenTypes.h"

class ExposeViewWindow;
class OSApp;
class ImagePrimitive;

class ExposeGroup {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  ExposeGroup();
  void CalculateCenterAndBounds();
  bool Intersects(const ExposeGroup& other) const;
  Vector2 MinMovementToResolveCollision(const ExposeGroup& other) const;
  void Move(const Vector2& displacement);

  Vector2 m_minBounds;
  Vector2 m_maxBounds;
  Vector2 m_center;
  std::shared_ptr<OSApp> m_app;
  std::shared_ptr<ImagePrimitive> m_icon;
  std::unordered_set<std::shared_ptr<ExposeViewWindow>> m_groupMembers;
};
