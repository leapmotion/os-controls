#pragma once
#include "Renderable.h"
#include "SceneGraphNode.h"

#include "Primitives.h"
#include "Color.h"

#include "Resource.h"
#include "GLShader.h"

#include "uievents/OSCDomain.h"
#include "interaction/HandLocationRecognizer.h"

#include <string>
#include <Animation.h>

class RenderEngine;

class CursorView :
  public std::enable_shared_from_this<CursorView>,
  public Renderable
{
public:
  CursorView(float radius, const Color& color);
  ~CursorView();
  
  void AutoInit();
  
  void SetSize(float radius);
  
  void AutoFilter(OSCState appState, const HandLocation& handLocation);
  
  void AnimationUpdate(const RenderFrame& frame);
  
  void Render(const RenderFrame& frame) const override;
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  enum class State {
    INACTIVE,
    ACTIVE
  };
  
  State m_state;
  Animated<float> m_opacity;
  
  Disk m_disk;
  
  Autowired<RenderEngine> m_renderEngine;
};
