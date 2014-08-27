#pragma once

#include "RenderEngine.h"
#include "RenderEngineNode.h"

/// <summary>
/// Implements expose view
/// </summary>
class ExposeView :
public RenderEngineNode
{
public:
  ExposeView(void);
  ~ExposeView(void);
  void AutoInit();
  
  void AnimationUpdate(const RenderFrame& frame);
  
  void CloseView();
  
private:
  Autowired<RootRenderEngineNode> m_rootNode;
};

