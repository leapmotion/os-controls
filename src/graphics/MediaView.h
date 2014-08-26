#pragma once
#include "graphics/Wedges.h"
#include "graphics/RenderEngineNode.h"
#include "graphics/VolumeControl.h"
#include "interaction/HandRollRecognizer.h"
#include "uievents/HandProperties.h"
#include "uievents/MediaViewEventListener.h"
//#include "utility/ExtendedStateMachine.h"
#include "SceneGraphNode.h"
#include "Leap.h"

#include "Primitives.h"
#include "Animation.h"
#include <array>

enum class HandPose;
enum class OSCState;
struct HandLocation;

class MediaView :
  public RenderEngineNode,
  public ContextMember
{
public:
  MediaView(const Vector3& center, float offset);
  
  void AutoInit();

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
  void SetVolumeView(float volume);
  void NudgeVolumeView(float dVolume);
  
  //All user and state machine driven changes to the view are dealt with from here.
  void AutoFilter(OSCState appState, const Leap::Frame& frame, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  void openMenu(const HandLocation& handLocation);
  void closeMenu();
  
  //Send direction about max opacity and goal opacity to the individual wedges.
  void setMenuOpacity(float opacity);

  /// <summary>
  /// Inputs _to the media view state machine_, NOT the AutoFilter routine.
  /// <summary>
  /// Decides which wedge is closest to the given point
  /// </summary>
  /// <remarks>
  /// This method will always return some value, even if the point is very far from all wedges
  /// or even in the dead zone
  /// </remarks>
  void updateWedges(const HandLocation& handLocation);
  float distanceFromCenter(const HandLocation& handLocation);
  std::shared_ptr<Wedge> closestWedgeToPoint(const HandLocation& handLocation);
  void updateWedgePositions(std::shared_ptr<Wedge> activeWedge, float distanceFromDeadzone);
  void checkForSelection(std::shared_ptr<Wedge> activeWedge, float distanceFromDeadzone);
  
  float calculateVolumeDelta(float deltaHandRoll);
  

  /// <summary>
  /// State for the wedge network on the media view control
  /// </summary>

  enum class State {
    
    /*                        |----------V
     *    --> Inactive --> Active --> SelectionMade
     *           ^-----------|-----------|
     */
    
    //Media View is created but not focused.
    INACTIVE,
    
    //Taking user input, fading in, etc
    ACTIVE,
    
    //Done taking input, has sent its event up the chain. Mostly for finished animations.
    SELECTION_MADE
  };
  
  State m_state;

  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;

  // True if the control is currently in the dead zone
  bool m_deadZone;

  // MediaView properties
  Animated<float> m_opacity;

  std::shared_ptr<Wedge> m_lastActiveWedge;

  //TODO: Get rid of magic number (which is the number of wedges
  std::array<std::shared_ptr<Wedge>, 4> m_wedges;
  std::shared_ptr<VolumeControl> m_volumeControl;
  
  Autowired<RenderEngineNode> m_rootNode;
};