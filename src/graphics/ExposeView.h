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
  /// Recovers a window from the specified abstract coordinates
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(double x, double y) const;
  
private:
  Autowired<RootRenderEngineNode> m_rootNode;
};

