#pragma once
#include "graphics/RadialButton.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"

#include "Primitives.h"


class MediaView :
  public RenderEngineNode {
public:
  MediaView(const Vector3& center, float offset);
  
  void InitChildren() override;
  void AnimationUpdate(const RenderFrame& frame) override;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  std::shared_ptr<RadialButton> m_leftButton;
  std::shared_ptr<RadialButton> m_topButton;
  std::shared_ptr<RadialButton> m_rightButton;

  std::shared_ptr<VolumeControl> m_volumeControl;

  float m_time;
};
