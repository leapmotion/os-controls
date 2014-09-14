#pragma once
#include "cursor/CursorView.h"
#include "graphics/Renderable.h"
#include "mediaview/VolumeSliderView.h"
#include "mediaview/MediaViewController.h"
#include "interaction/HandDataCombiner.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/OSCDomain.h"

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
  const float MENU_RADIUS = 180.0f;
  const float ACTIVATION_RADIUS = MENU_RADIUS + 100.0f;
  const float MENU_THICKNESS = 100.0f;
  const Vector3 VOLUME_SLIDER_OFFSET = Vector3( 0.0f, 200.0f, 0.0f );
  const Color GHOST_CURSOR_COLOR = Color( 0.505f, 0.831f, 0.114f );
  const float GHOST_CURSOR_ALPHA = 0.3f;
  
  void resetMemberState();
  void doMenuUpdate(const Vector2& locationData, Vector2 menuOffset);
  void doVolumeUpdate(const Vector2& locationData, const Vector2& deltaPixels, Vector2 menuOffset);
  void resolveSelection(int selectedID);
  //Adjust the view for the volume control
  float calculateVolumeDelta(float deltaHandRoll);
  Vector3 calculateBufferZoneOffset(const Vector2& screenPosition);
  
  
  /// <summary>
  /// State for the wedge network on the media view control
  /// </summary>
  
  enum class State {
    // Media View is created but not focused.
    // It is wainting for focus to activate.
    ARMED,
    
    // Taking user input, fading in, etc
    ACTIVE,
    
    // The menu's selection action has been made.
    // This instance of the interaction is done.
    // Wait for animation to fade out.
    COMPLETE,
    
    // Tear everything down.
    FINAL
  };
  
  std::shared_ptr<RadialMenu> m_radialMenu;
  std::shared_ptr<VolumeSliderView> m_volumeSlider;
  std::shared_ptr<Disk> m_ghostCursor;
  
  Smoothed<float> m_ghostCursorAlpha;
  Smoothed<float> m_volumeViewAlpha;
  
  HandPose m_lastHandPose;
  
  State m_state;
  
  // Refernce to the cursor so we can override its position
  Autowired<CursorView> m_cursorView;
  Autowired<sf::RenderWindow> m_renderWindow;
  
  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
  AutoFired<OSCStateChangeEvent> m_stateEvents;
  
  Autowired<RenderEngine> m_rootNode;
  RectanglePrim prim;
  
  // How much do we want to offset the cursor to keep
  // interactions with the radial menu within
  // the screen's "buffer zone"
  Vector3 m_cursorBufferzoneOffset;

  int m_selectedItem;
  double m_FadeTime;
  double m_CurrentTime;
  double m_LastStateChangeTime;
  bool m_interactionIsVolumeLocked;
};