#pragma once

#include "Primitives.h"
#include "SVGPrimitive.h"
#include "Color.h"
#include "Animation.h"
#include "RadialItem.h"

class RadialMenuItem : public RadialItem  {
public:

  RadialMenuItem();
  //void SetActivation(double activation) { m_Activation = activation; }
  void SetActivation(double activation) { m_Activation.SetGoal(activation); }
  void SetActivatedRadius(double radius) { m_ActivatedRadius = radius; }

  void SetActivatedColor(const Color& activatedColor) { m_ActivatedColor = activatedColor; }
  void SetHoverColor(const Color& hoverColor) { m_HoverColor = hoverColor; }

  void SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon);

  bool Hit(const Vector2& pos, double& ratio) const;

  virtual void Draw(RenderState& renderState) const override;

  double CurrentRadius() const;

  void UpdateActivation(float deltaTime) { m_Activation.Update(deltaTime); }
  double CurrentActivation() const { return m_Activation.Value(); }

protected:
  Color calculateColor() const;

  Smoothed<double> m_Activation;
  double m_ActivatedRadius;

  Color m_ActivatedColor;
  Color m_HoverColor;

  double m_IconScale;
  Vector3 m_IconOffset;
  std::shared_ptr<SVGPrimitive> m_Icon;

  mutable std::shared_ptr<PartialDiskWithTriangle> m_Wedge;
  mutable std::shared_ptr<PartialDiskWithTriangle> m_Goal;
};

class RadialMenu : public RadialItem  {
public:
  struct UpdateResult {
    UpdateResult(int _updateIdx, double _curActivation) : updateIdx(_updateIdx), curActivation(_curActivation) { }
    int updateIdx;
    double curActivation;
  };

  struct HitResult {
    HitResult(int _hitIdx, double _hitRatio) : hitIdx(_hitIdx), hitRatio(_hitRatio) { }
    int hitIdx;
    double hitRatio;
  };

  RadialMenu();
  void SetNumItems(int num);
  const std::shared_ptr<RadialMenuItem>& GetItem(int i) const { return m_Items[i]; }
  std::shared_ptr<RadialMenuItem>& GetItem(int i) { return m_Items[i]; }
  UpdateResult UpdateItemsFromCursor(const Vector3& cursor, float deltaTime);
  HitResult ItemFromPoint(const Vector2& pos) const;
  virtual void Draw(RenderState& renderState) const override;
protected:
  void updateItemLayout();

  std::vector<std::shared_ptr<RadialMenuItem>> m_Items;
};
