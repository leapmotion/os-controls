#pragma once
#include "graphics/Renderable.h"
#include "interaction/HandLocationRecognizer.h"
#include "interaction/FrameDeltaTimeRecognizer.h"
#include "interaction/HandPoseRecognizer.h"
#include "interaction/HandRollRecognizer.h"
#include "interaction/MediaViewController.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/OSCDomain.h"
#include "VolumeKnob.h"
#include <RadialMenu.h>
#include <RadialSlider.h>
#include <autowiring/Autowiring.h>

class RenderEngine;

class MediaViewStateMachine :
  public std::enable_shared_from_this<MediaViewStateMachine>,
  public Renderable
{
public:
  MediaViewStateMachine();
  virtual ~MediaViewStateMachine() {}
  
  void AutoInit();
  
  //All user and state machine driven changes to the view are dealt with from here.
  void AutoFilter(OSCState appState, const DeltaRollAmount& dra, const HandLocation& handLocation, const HandPose& handPose, const FrameTime& frameTime);

  void AnimationUpdate(const RenderFrame& renderFrame) override;
  void Render(const RenderFrame& renderFrame) const override;
  void SetViewVolume(float volume);
  
private:
  void resolveSelection(int selectedID);
  //Adjust the view for the volume control
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
    SELECTION_MADE,
    
    //Wait for animation to fade out
    FADE_OUT,
    
    //Tear everything down.
    FINAL
  };
  
  std::shared_ptr<RadialMenu>m_radialMenu;
  std::shared_ptr<RadialSlider> m_volumeSlider;
  AutoRequired<VolumeKnob> m_volumeKnob;
  
  bool m_hasRoll;
  float m_startRoll;
  HandPose m_lastHandPose;
  
  State m_state;
  
  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
  
  Autowired<RenderEngine> m_rootNode;
  RectanglePrim prim;

  int m_selectedItem;
  double m_FadeTime;
  double m_CurrentTime;
  double m_LastStateChangeTime;
};