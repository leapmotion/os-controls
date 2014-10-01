#pragma once
#include "cursor/CursorView.h"
#include "graphics/Renderable.h"
#include "mediaview/VolumeSliderView.h"
#include "mediaview/MediaViewController.h"
#include "interaction/HandDataCombiner.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/ShortcutsDomain.h"

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
  void AutoFilter(ShortcutsState appState, const HandData& handData, const FrameTime& frameTime);

  void AnimationUpdate(const RenderFrame& renderFrame) override;
  void Render(const RenderFrame& renderFrame) const override;
  bool IsVisible() const override { return m_state == State::ACTIVE || m_state == State::LOCKED; }

  void SetViewVolume(float volume);
  
private:
  const float MENU_RADIUS = 180.0f;
  const float ACTIVATION_RADIUS = MENU_RADIUS + 100.0f;
  const float MENU_THICKNESS = 100.0f;
  const Vector3 VOLUME_SLIDER_OFFSET = Vector3( 0.0f, 200.0f, 0.0f );
  const float VOLUME_OFFSET_START_Y = 80.0f;
  const float VOLUME_LOCK_IN_Y = 180.0f;
  const float VOLUME_LOCK_X_OFFSET = 35.0f;
  const float KILL_FADE_START_DISTANCE = 320.0f;
  const float KILL_FADE_END_DISTANCE = 420.0f;
  
  float calculateMenuAlphaFade();
  bool shouldMenuDistanceKill();
  void doActiveToLockedTasks();
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
    
    // Don't take new input on the radial menu. Reset the menu.
    LOCKED,
    
    // We've "distance killed" and don't want the menu to come
    // back up until we've re-initialized
    DISTANCE_KILLED,
    
    // Tear everything down.
    // Watch it burn.
    FINAL
  };
  
  std::shared_ptr<RadialMenu> m_radialMenu;
  std::shared_ptr<VolumeSliderView> m_volumeSlider;

  Smoothed<float> m_volumeViewAlpha;
  
  HandPose m_lastHandPose;
  
  State m_state;
  
  // Refernce to the cursor so we can override its position
  Autowired<CursorView> m_cursorView;
  Autowired<sf::RenderWindow> m_renderWindow;
  
  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
  AutoFired<ShortcutsStateChangeEvent> m_stateEvents;
  
  Autowired<RenderEngine> m_rootNode;
  RectanglePrim prim;
  
  // How much do we want to offset the cursor to keep
  // interactions with the radial menu within
  // the screen's "buffer zone"
  Vector3 m_cursorBufferzoneOffset;
  
  // When we're active we're offsetting the cursor position.
  // This is where we want to put it.
  Vector2 m_goalCursorPosition;

  int m_selectedItem;
  double m_FadeTime;
  double m_CurrentTime;
  double m_LastStateChangeTime;
  bool m_interactionIsLocked;
  float m_distanceFadeCap;
};
