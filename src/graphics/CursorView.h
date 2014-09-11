#pragma once
#include "Renderable.h"
#include "SceneGraphNode.h"

#include "Primitives.h"
#include "Color.h"

#include "Resource.h"
#include "GLShader.h"

#include "uievents/OSCDomain.h"
#include "interaction/HandDataCombiner.h"
#include "interaction/TimeRecognizer.h"
#include "osinterface/OSWindowMonitor.h"
#include "osinterface/OSWindow.h"

#include "HandCursor.h"

#include <string>
#include <SVGPrimitive.h>
#include <Animation.h>

class RenderEngine;

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
  
  // Handle all the visual updates that benefit from running on a graphics tick versus the input loop.
  void AnimationUpdate(const RenderFrame& frame);
  
  void Render(const RenderFrame& frame) const override;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  enum class State {
    INACTIVE,
    ACTIVE
  };
  
  Vector2 getWindowCenter(OSWindow& window);
  
  State m_state;
  
  Vector2 m_scrollBodyOffset;
  Vector2 m_scrollLineOffset;
  Vector2 m_scrollFingerLeftOffset;
  Vector2 m_scrollFingerRightOffset;
  
  std::shared_ptr<SVGPrimitive> m_scrollBody;
  std::shared_ptr<SVGPrimitive> m_scrollLine;
  std::shared_ptr<SVGPrimitive> m_scrollFingerLeft;
  std::shared_ptr<SVGPrimitive> m_scrollFingerRight;
  
  std::shared_ptr<Disk> m_disk;
  
  Autowired<OSWindowMonitor> m_osWindowMonitor;
  
  std::shared_ptr<OSWindow> m_lastSelectedWindow;
  
  float m_fingerSpread;
  float m_pinchStrength;
  bool m_wasPinching;
  Vector2 m_lastHandPosition;
  
  bool m_isPointing;
  
  Smoothed<float> m_bodyOffset;
  Smoothed<float> m_x;
  Smoothed<float> m_y;
  Smoothed<float> m_ghostX;
  Smoothed<float> m_ghostY;
  Smoothed<float> m_bodyAlpha;
  
  Autowired<RenderEngine> m_renderEngine;
};
