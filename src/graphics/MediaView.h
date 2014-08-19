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
  
  enum FadeState {
    FADE_OUT,
    FADE_IN
  };
  void SetFadeState(FadeState newState);
  
  float Volume();
  void SetVolume(float volume);
  void NudgeVolume(float dVolume);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  
  void setOpacity(float opacity);

  float m_time;
  FadeState m_fadeState;

  std::shared_ptr<RadialButton> m_leftButton;
  std::shared_ptr<RadialButton> m_topButton;
  std::shared_ptr<RadialButton> m_rightButton;
  std::shared_ptr<VolumeControl> m_volumeControl;
};
