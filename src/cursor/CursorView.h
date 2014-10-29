#pragma once

#include "graphics/Renderable.h"
#include "interaction/HandDataCombiner.h"
#include "interaction/TimeRecognizer.h"
#include "osinterface/OSWindowMonitor.h"
#include "osinterface/OSWindow.h"
#include "uievents/ShortcutsDomain.h"

#include "Animation.h"
#include "Color.h"
#include "HandCursor.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/SceneGraphNode.h"
#include "Primitives.h"
#include "Resource.h"

#include <string>
#include <SVGPrimitive.h>

class RenderEngine;
class Config;

class CursorView :
  public std::enable_shared_from_this<CursorView>,
  public Renderable
{
public:
  CursorView();
  ~CursorView();

  void AutoInit();

  void SetSize(float radius);

  // Input logic
  void AutoFilter(const Leap::Hand& hand, ShortcutsState appState, const HandData& handData, const FrameTime& frameTime);

  // Get where the cursor thinks it should be (this will be its position unless it is being overriden)
  EigenTypes::Vector2 GetCalculatedLocation() const;

  void Disable() { m_state = State::DISABLED; }
  void EnableMediaView() { m_state = State::DISK; }
  void EnableHandAndScroll() { m_state = State::HAND; }

  void EnableLocationOverride() { m_overrideInfluence = 1.0f; }
  void DisableLocationOverride() { m_overrideInfluence = 0.0f; }
  void SetOverideLocation(const EigenTypes::Vector2& offsetLocation);

  // Implement Renderable
  void AnimationUpdate(const RenderFrame& frame);  // Handle all the visual updates that benefit from running on a graphics tick versus the input loop.
  void Render(const RenderFrame& frame) const override;
  bool IsVisible() const override { return m_state == State::DISK || m_state == State::HAND; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  enum class State {
    INACTIVE,
    DISABLED,
    DISK,
    HAND
  };

  const float MAX_CURSOR_SMOOTHING = 0.6f;

  //The mm delta that will result in a smoothing factor of 0.0 and MAX_CURSOR_SMOOTHING respectively
  const float DELTA_FOR_MIN_SMOOTHING = 15.0f;
  const float DELTA_FOR_MAX_SMOOTHING = 3.0f;

  float calcPositionSmoothStrength(float handDeltaDistance) const;
  EigenTypes::Vector2 getWindowCenter(OSWindow& window);

  State m_state;

  EigenTypes::Vector2 m_scrollBodyOffset;
  EigenTypes::Vector2 m_scrollLineOffset;
  EigenTypes::Vector2 m_scrollFingerLeftOffset;
  EigenTypes::Vector2 m_scrollFingerRightOffset;
  EigenTypes::Vector2 m_disabledCursorOffset;

  std::shared_ptr<SVGPrimitive> m_scrollBody;
  std::shared_ptr<SVGPrimitive> m_scrollLine;
  std::shared_ptr<SVGPrimitive> m_scrollFingerLeft;
  std::shared_ptr<SVGPrimitive> m_scrollFingerRight;
  std::shared_ptr<SVGPrimitive> m_disabledCursor;

  // The cursor that shows up when not scrolling
  std::shared_ptr<Disk> m_disk;
  std::shared_ptr<HandCursor> m_handCursor;

  Autowired<OSWindowMonitor> m_osWindowMonitor;
  Autowired<Config> m_config;

  std::shared_ptr<OSWindow> m_lastSelectedWindow;

  float m_fingerSpread;
  float m_pinchStrength;
  EigenTypes::Vector2 m_lastHandDeltas;
  EigenTypes::Vector2 m_lastHandPosition;
  EigenTypes::Vector3 m_lastHandVelocity;

  ShortcutsState m_lastAppState;

  Smoothed<float> m_bodyOffset;
  Smoothed<float> m_x;
  Smoothed<float> m_y;
  float m_overrideX;
  float m_overrideY;
  float m_overrideInfluence; // how much do we follow the offset amount.
  Smoothed<float> m_bodyAlpha;
  Smoothed<float> m_diskAlpha;

  Autowired<RenderEngine> m_renderEngine;
};
