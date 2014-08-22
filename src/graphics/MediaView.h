#pragma once
#include "MediaViewEventListener.h"
#include "graphics/RadialButton.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"
#include "graphics/MostRecent.h"
#include "interaction/HandExistTrigger.h"
#include "SceneGraphNode.h"
#include "Leap.h"

#include "Primitives.h"
#include "Animation.h"

class MediaView :
  public RenderEngineNode
{
public:
  MediaView(const Vector3& center, float offset);
  
  void InitChildren() override;
  void AnimationUpdate(const RenderFrame& frame) override;
  int setActiveWedgeFromPoint(const Vector2& point);
  void Move(const Vector3& coords);
  void SetInteractionDistance(float distance);
  void DeselectWedges();
  void CloseMenu(double selectionCloseDelayTime = false); // override that leaves the current active wedge visible for a given time
  
  void SetGoalOpacity(float goalOpacity);
  void FadeIn(){ SetGoalOpacity(config::MEDIA_BASE_OPACITY); }
  void FadeOut() { SetGoalOpacity(0.0f); }

  float Volume();
  void SetVolume(float volume);
  void NudgeVolume(float dVolume);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  void setMenuOpacity(float opacity);

  AutoFired<MediaViewEventListener> m_mve;
  Animated<float> m_opacity;
  
  float m_interactionDistance;
  std::shared_ptr<RadialButton> m_activeWedge;

  std::vector<std::shared_ptr<RadialButton>> m_wedges; //0 - Top, 1 - Right, 2 - Down, 3 - Left
  std::shared_ptr<VolumeControl> m_volumeControl;
};