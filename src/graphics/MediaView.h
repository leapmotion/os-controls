#pragma once
#include "graphics/RadialButton.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"
#include "graphics/MostRecent.h"
#include "interaction/HandExistTrigger.h"
#include "interaction/RollDetector.h"
#include "uievents/AbstractVolumeControl.h"
#include "uievents/HandProperties.h"
#include "uievents/MediaViewEventListener.h"
#include "utility/ExtendedStateMachine.h"
#include "SceneGraphNode.h"
#include "Leap.h"

#include "Primitives.h"
#include "Animation.h"
#include <array>

enum class HandPose;
enum class OSCState;
struct HandLocation;

class MediaView :
  public RenderEngineNode
{
public:
  MediaView(const Vector3& center, float offset);
  
  //Create the sub-nodes of our current render node.
  void InitChildren() override;
  
  //Non-User-Input driven animation and rendering update (eg. fade-in and fade-out)
  void AnimationUpdate(const RenderFrame& frame) override;
  
  //Set the translation portion of our node's tranform.
  void Move(const Vector3& coords);
  
  //Set the overall opacity of the menu (This is the high bound for opacity)
  void SetGoalOpacity(float goalOpacity);

  //Wrappers on some fading in and out logic.
  void FadeIn(){ SetGoalOpacity(config::MEDIA_BASE_OPACITY); }
  void FadeOut() { SetGoalOpacity(0.0f); }

  //Adjust the view for the volume control
  void SetVolumeView(float volume) override;
  void NudgeVolumeView(float dVolume) override;
  
  //All user and state machine driven changes to the view are dealt with from here.
  void AutoFilter(OSCState state, const Leap::Frame& frame, const HandLocation& handLocation, const HandRoll& dHandRoll);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  //Send direction about max opacity and goal opacity to the individual wedges.
  void setMenuOpacity(float opacity);

  //Names for all the wedges
  enum class Wedge {
    Top,
    Right,
    Down,
    Left,

    // Utility member, used to size this enum--equal to 4
    WEDGE_SIZE
  };

  /// <summary>
  /// Decides which wedge is closest to the given point
  /// </summary>
  /// <remarks>
  /// This method will always return some value, even if the point is very far from all wedges
  /// or even in the dead zone
  /// </remarks>
  Wedge wedgeFromPoint(const Vector2& point) const;

  /// <returns>
  /// The wedge corresponding to the specified wedge name
  /// </returns>
  std::shared_ptr<RadialButton>& wedge(Wedge wedge) { return m_wedges[(int) wedge]; }

  // State machine evaluation functions:
  bool isPointInDeadZone(const Vector2& userPosition) const;

  /// <summary>
  /// State for the wedge network on the media view control
  /// </summary>
  enum class State {/*
    // Means that this media view is not visible right now.  All other states have the
    // media control represented as visible.
    Invisible,

    // Nothing interesting is happening.  We "count" as being visible but the user is
    // not currently interacting.  Even if we're presently fading out, this will continue
    // to be our current state until we are actually faded out completely.
    DeadZone,

    // User is asking to alter the volume by some epsilon amount
    AlteringVolume,

    // Not in the dead zone, interacting with a wedge somewhere, or maybe more than one
    // wedge.
    AlteringWedge,

    // A selection has been made.  We continue to allow interactions in this state but we
    // don't actually take any action involving wedges.
    SelectionIndicated*/
    
    //Taking user input, fading in, etc
    Active,
    
    //Done taking input, has sent its event up the chain. Mostly for finished animations.
    Finished
  };

  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;

  // True if the control is currently in the dead zone
  bool m_deadZone;

  // MediaView properties
  Animated<float> m_opacity;
  float m_interactionDistance;

  std::shared_ptr<RadialButton> m_activeWedge;
  std::array<std::shared_ptr<RadialButton>, (int) Wedge::WEDGE_SIZE> m_wedges;
  std::shared_ptr<VolumeControl> m_volumeControl;
};