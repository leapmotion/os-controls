#pragma once
#include "graphics/Renderable.h"
#include "interaction/MediaViewController.h"
#include "interaction/HandDataCombiner.h"
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
  void AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime);

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
    
    // Media View is created but not focused.
    // It is wainting for focus to activate.
    ARMED,
    
    //Taking user input, fading in, etc
    ACTIVE,
    
    // The menu's selection action has been made.
    // This instance of the interaction is done.
    // Wait for animation to fade out.
    COMPLETE,
    
    // Tear everything down.
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
  AutoFired<OSCStateChangeEvent> m_stateEvents;
  
  Autowired<RenderEngine> m_rootNode;
  RectanglePrim prim;

  int m_selectedItem;
  double m_FadeTime;
  double m_CurrentTime;
  double m_LastStateChangeTime;
};