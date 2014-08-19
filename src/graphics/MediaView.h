#pragma once
#include "graphics/RadialButton.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"
#include "graphics/MostRecent.h"
#include "interaction/HandExistTrigger.h"
#include "SceneGraphNode.h"
#include "Leap.h"

#include "Primitives.h"


class MediaView :
public RenderEngineNode {
public:
  MediaView(const Vector3& center, float offset);
  
  void InitChildren() override;
  void AnimationUpdate(const RenderFrame& frame) override;
  int setActiveWedgeFromPoint(const Vector2& point);
  void Move(const Vector3& coords);
  void SetInteractionDistance(float distance);
  void DeselectWedges();
  void CloseMenu(double selectionCloseDelayTime = false); // override that leaves the current active wedge visible for a given time
  
  enum FadeState {
    FADE_OUT,
    FADE_IN,
    INVISIBLE
  };
  
  void SetFadeState(FadeState newState);
  
  float Volume();
  void SetVolume(float volume);
  void NudgeVolume(float dVolume);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  void setMenuOpacity(float opacity);

  float m_time;
  float m_opacity;
  FadeState m_fadeState;
  
  float m_interactionDistance;
  std::shared_ptr<RadialButton> m_activeWedge;

  std::vector<std::shared_ptr<RadialButton>> m_wedges; //0 - Top, 1 - Right, 2 - Down, 3 - Left
  std::shared_ptr<VolumeControl> m_volumeControl;
};
