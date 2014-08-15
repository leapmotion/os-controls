#pragma once
#include "graphics/RadialButton.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"
#include "graphics/MostRecent.h"
#include "interaction/HandExistTrigger.h"
#include "Leap.h"

#include "Primitives.h"


class MediaView :
public RenderEngineNode {
public:
  MediaView(const Vector3& center, float offset);
  
  void InitChildren() override;
  void AnimationUpdate(const RenderFrame& frame) override;
  
  void setOpacity(float opacity);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  std::shared_ptr<RadialButton> m_leftButton;
  std::shared_ptr<RadialButton> m_topButton;
  std::shared_ptr<RadialButton> m_rightButton;
  std::shared_ptr<VolumeControl> m_volumeControl;
  
  float m_time;
  float m_scale;
};
