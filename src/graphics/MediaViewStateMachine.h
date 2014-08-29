#pragma once
#include "MediaView.h"

class MediaViewStateMachine {
public:
  MediaViewStateMachine();
  virtual ~MediaViewStateMachine() {};
  
  //All user and state machine driven changes to the view are dealt with from here.
  void AutoFilter(OSCState appState, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll);
  
  
private:
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
  
  State m_state;
  
  Autowired<MediaView> m_mediaView;
  
  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
  
  AutoRequired<MediaViewController> m_mediaViewController;
  std::shared_ptr<Wedge> m_lastActiveWedge;
};