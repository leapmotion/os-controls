#pragma once

#include "Primitives.h"
#include "SVGPrimitive.h"
#include "Animation.h"
#include "RadialItem.h"
#include "Leap/GL/Rgba.h"

using namespace Leap::GL;

class RadialMenuItemEvent {
public:
  virtual void OnActivated() { }
};

class RadialMenuItem : public RadialItem  {
public:

  RadialMenuItem();
  void InitChildren();
  //void SetActivation(double activation) { m_Activation = activation; }
  void SetActivation(double activation) { m_Activation.SetGoal(activation); }
  void SetActivatedRadius(double radius) { m_ActivatedRadius = radius; }

  void SetActivatedColor(const Rgba<float>& activatedColor) { m_ActivatedColor = activatedColor; }
  void SetHoverColor(const Rgba<float>& hoverColor) { m_HoverColor = hoverColor; }

  void SetIcon(const std::shared_ptr<SVGPrimitive>& svgIcon);

  bool Hit(const EigenTypes::Vector2& pos, double& ratio) const;

  double CurrentRadius() const;

  void UpdateActivation(float deltaTime) { m_Activation.Update(deltaTime); }
  double CurrentActivation() const { return m_Activation.Value(); }

  void SetCallback(RadialMenuItemEvent* callback) { m_Callback = callback; }
  void CheckFireCallback();

protected:
  virtual void DrawContents(RenderState& renderState) const override;
  Rgba<float> calculateColor() const;

  Smoothed<double> m_Activation;
  double m_ActivatedRadius;

  Rgba<float> m_ActivatedColor;
  Rgba<float> m_HoverColor;

  double m_IconScale;
  EigenTypes::Vector3 m_IconOffset;
  std::shared_ptr<SVGPrimitive> m_Icon;

  RadialMenuItemEvent* m_Callback;
  bool m_Cooldown;

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
  UpdateResult InteractWithCursor(const EigenTypes::Vector3& cursor);
  void InteractWithoutCursor();
  void UpdateItemActivation(float deltaTime);
  HitResult ItemFromPoint(const EigenTypes::Vector2& pos) const;
protected:
  virtual void DrawContents(RenderState& renderState) const override;
  void updateItemLayout();

  std::vector<std::shared_ptr<RadialMenuItem>> m_Items;
};
