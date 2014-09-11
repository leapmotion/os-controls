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
  
  void AutoFilter(const Leap::Hand& hand, OSCState appState, const HandData& handData, const FrameTime& frameTime);
  
  void AnimationUpdate(const RenderFrame& frame);
  
  void Render(const RenderFrame& frame) const override;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  enum class State {
    INACTIVE,
    ACTIVE
  };
  
  State m_state;
  Animated<float> m_alphaMask;
  
  Vector2 m_scrollBodyOffset;
  Vector2 m_scrollLineOffset;
  Vector2 m_scrollFingerLeftOffset;
  Vector2 m_scrollFingerRightOffset;
  
  std::shared_ptr<SVGPrimitive> m_scrollBody;
  std::shared_ptr<SVGPrimitive> m_scrollLine;
  std::shared_ptr<SVGPrimitive> m_scrollFingerLeft;
  std::shared_ptr<SVGPrimitive> m_scrollFingerRight;
  
  float m_fingerSpread;
  Vector2 m_lastHandPosition;
  
  Smoothed<float> m_bodyOffset;
  Smoothed<float> m_x;
  Smoothed<float> m_y;
  
  Autowired<RenderEngine> m_renderEngine;
};
