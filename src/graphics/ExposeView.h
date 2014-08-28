#pragma once
#include "RenderEngine.h"
#include "RenderEngineNode.h"
#include "Primitives.h"
#include <Animation.h>
#include <vector>

class ExposeViewWindow;

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
  void moveWindowToTop(std::shared_ptr<RectanglePrim> window);

  Autowired<RootRenderEngineNode> m_rootNode; //Root node in the render tree
  bool m_handIsGrabbing; //Flag to know if the hand is currently grabbing
  Animated<float> m_opacity;
  
  // All windows currently known to this view:
  std::vector<std::shared_ptr<ExposeViewWindow>> m_windows;
  
  //Background Overlay Rectangle
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
  /// </summary>
  void StartView();
  void CloseView();

  // Should be called when the proxy hand begins a grab.
  // Handles the actual view response to the grab.
  // Sets state flags that impact the behavior of UpdateLayout
  void StartGrab();

  // Should be called when the proxy hand ends a grab.
  // Handles the actual view response to the grab release.
  // Sets state flags that impact the behavior of UpdateLayout
  // Returns whether or not the grab end should close the menu.
  // True -> Menu Should Close, False -> Menu should stay open.
  bool EndGrab();

  /// <summary>
  /// Recovers a window from the specified abstract coordinates
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(double x, double y) const;
};

