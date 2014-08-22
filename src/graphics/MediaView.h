#pragma once
#include "graphics/RadialButton.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"
#include "graphics/MostRecent.h"
#include "interaction/HandExistTrigger.h"
#include "uievents/AbstractVolumeControl.h"
#include "uievents/HandProperties.h"
#include "uievents/MediaViewEventListener.h"
#include "SceneGraphNode.h"
#include "Leap.h"

#include "Primitives.h"
#include "Animation.h"

enum class HandPose;
enum class OSCState;
struct HandLocation;

class MediaView :
  public AbstractVolumeControl,
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

  float Volume() override;
  void SetVolume(float volume) override;
  void NudgeVolume(float dVolume) override;
  
  void AutoFilter(OSCState state, const HandLocation& handLocation, const HandPose& handPose);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  void setMenuOpacity(float opacity);

  void updateWedges(void);

  /// <summary>
  /// Inputs _to the media view state machine_, NOT the AutoFilter routine.
  /// <summary>
  /// <remarks>
  /// These are facts about the inputs received by the AutoFilter routine.
  /// </remarks>
  enum class InputAlphabet {
    // The user has made a selection of the NEXT menu
    SelectedNext
  };

  enum class State {
    // Means that this media view is not visible right now
    Invisible,

    // Nothing interesting is happening.  We "count" as being visible but the user is
    // not currently interacting.  Even if we're presently fading out, this will continue
    // to be our current state until we are actually faded out completely.
    Visible,

    // The user is messing with one of the wedges
    AlteringWedges
  };
  State m_state;

  // Last hand roll amount, used to guard against wander
  float m_lastRoll;

  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mve;

  // MediaView properties:
  Animated<float> m_opacity;
  float m_interactionDistance;


  std::shared_ptr<RadialButton> m_activeWedge;
  std::vector<std::shared_ptr<RadialButton>> m_wedges; //0 - Top, 1 - Right, 2 - Down, 3 - Left
  std::shared_ptr<VolumeControl> m_volumeControl;
};