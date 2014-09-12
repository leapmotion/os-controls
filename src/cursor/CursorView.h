#pragma once

#include "graphics/Renderable.h"
#include "interaction/HandLocationRecognizer.h"
#include "interaction/HandPoseRecognizer.h"
#include "uievents/OSCDomain.h"

#include "Animation.h"
#include "Color.h"
#include "GLShader.h"
#include "HandCursor.h"
#include "Primitives.h"
#include "Resource.h"
#include "SceneGraphNode.h"

#include <string>

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
  
  void AutoFilter(const Leap::Hand& hand, OSCState appState, const HandPose& handPose, const HandLocation& handLocation);
  
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
  
  std::shared_ptr<HandCursor> m_handCursor;
  
  Autowired<RenderEngine> m_renderEngine;
};
