#pragma once

#include "graphics/Renderable.h"
#include "interaction/HandDataCombiner.h"
#include "interaction/TimeRecognizer.h"
#include "osinterface/OSWindowMonitor.h"
#include "osinterface/OSWindow.h"
#include "uievents/OSCDomain.h"

#include "Animation.h"
#include "Color.h"
#include "GLShader.h"
#include "HandCursor.h"
#include "Primitives.h"
#include "Resource.h"
#include "SceneGraphNode.h"

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
  void AutoFilter(const Leap::Hand& hand, OSCState appState, const HandData& handData, const FrameTime& frameTime);
  
  // Get where the cursor thinks it should be (this will be its position unless it is being overriden)
  Vector2 GetCalculatedLocation() const;
  
  void Disable() { m_state = State::DISABLED; }
  void Enable() { m_state = State::ACTIVE; }
  
  void EnableLocationOverride() { m_overrideInfluence = 1.0f; }
  void DisableLocationOverride() { m_overrideInfluence = 0.0f; }
  void SetOverideLocation(const Vector2& offsetLocation);
  
  // Implement Renderable
  void AnimationUpdate(const RenderFrame& frame);  // Handle all the visual updates that benefit from running on a graphics tick versus the input loop.
  void Render(const RenderFrame& frame) const override;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  enum class State {
    INACTIVE,
    DISABLED,
    ACTIVE
  };
  
  const float MAX_CURSOR_SMOOTHING = 0.6f;
  
  //The mm delta that will result in a smoothing factor of 0.0 and MAX_CURSOR_SMOOTHING respectively
  const float DELTA_FOR_MIN_SMOOTHING = 15.0f;
  const float DELTA_FOR_MAX_SMOOTHING = 3.0f;
  
  float calcPositionSmoothStrength(float handDeltaDistance) const;
  Vector2 getWindowCenter(OSWindow& window);
  
  State m_state;
  
  Vector2 m_scrollBodyOffset;
  Vector2 m_scrollLineOffset;
  Vector2 m_scrollFingerLeftOffset;
  Vector2 m_scrollFingerRightOffset;
  Vector2 m_disabledCursorOffset;
  
  std::shared_ptr<SVGPrimitive> m_scrollBody;
  std::shared_ptr<SVGPrimitive> m_scrollLine;
  std::shared_ptr<SVGPrimitive> m_scrollFingerLeft;
  std::shared_ptr<SVGPrimitive> m_scrollFingerRight;
  std::shared_ptr<SVGPrimitive> m_disabledCursor;
  
  // The cursor that shows up when not scrolling
  std::shared_ptr<Disk> m_disk;
  
  Autowired<OSWindowMonitor> m_osWindowMonitor;
  Autowired<Config> m_config;

  std::shared_ptr<OSWindow> m_lastSelectedWindow;
  
  float m_fingerSpread;
  float m_pinchStrength;
  Vector2 m_lastHandDeltas;
  Vector2 m_lastHandPosition;
  
  OSCState m_lastAppState;
  
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
