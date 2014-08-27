#pragma once
#include "RenderEngine.h"
#include "RenderEngineNode.h"

class ExposeViewWindow;

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

  /// <summary>
  /// 
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(uint32_t x, uint32_t y) const;
  
private:
  Autowired<RootRenderEngineNode> m_rootNode;
};

