#pragma once
#include "RenderEngine.h"
#include "RenderEngineNode.h"
#include "Primitives.h"
#include <Animation.h>
#include <vector>

class ExposeViewWindow;
class OSWindow;

/// <summary>
/// Implements expose view
/// </summary>
class ExposeView:
  public RenderEngineNode
{
public:
  ExposeView(void);
  ~ExposeView(void);
  void AutoInit();
  
private:
  // Find the given window in the window list and move
  // to the end of the vector such that it will render on top.
  void moveWindowToTop(ExposeViewWindow& window);

  //Root node in the render tree
  Autowired<RootRenderEngineNode> m_rootNode;

  // Opacity value for the entire view
  Animated<float> m_opacity;
  
  // All windows currently known to this view:
  std::vector<std::shared_ptr<ExposeViewWindow>> m_windows;

  // Rendering order:
  std::list<ExposeViewWindow*> m_renderList;
  
  // Background Overlay Rectangle
  RectanglePrim m_backgroundRect;

private:
  /// <summary>
  /// Evolves the layout by one step
  /// </summary>
  void UpdateLayout(std::chrono::duration<double> timeout);

public:
  // RenderEngineNode overrides:
  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;

  /// <summary>
  /// Creates a new ExposeViewWindow for the specified OS window
  /// </summary>
  std::shared_ptr<ExposeViewWindow> NewExposeWindow(OSWindow& osWindow);

  /// <summary>
  /// </summary>
  void StartView();
  void CloseView();

  /// <summary>
  /// Recovers a window from the specified abstract coordinates
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(double x, double y) const;
};

