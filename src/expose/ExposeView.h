#pragma once
#include "Primitives.h"
#include "ExposeViewWindow.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderEngineNode.h"
#include <Animation.h>
#include <vector>
#include <tuple>

class ExposeViewWindow;
class ExposeViewEvents;
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
  
  //Events to send to controller
  AutoFired<ExposeViewEvents> m_exposeViewEvents;

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
  void updateLayout(std::chrono::duration<double> dt);
  
  // Send commend to controller to focus the given window.
  void focusWindow(ExposeViewWindow& windowToFocus);
  
  // Convert a radian angle and a pixel distance to a point.
  // Returns a tuple x,y
  std::tuple<double, double> radialCoordsToPoint(double angle, double distance);

public:
  // RenderEngineNode overrides:
  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;

  /// <summary>
  /// Creates a new ExposeViewWindow for the specified OS window
  /// </summary>
  std::shared_ptr<ExposeViewWindow> NewExposeWindow(OSWindow& osWindow);

  /// <summary>
  /// Removes the specified expose view window from the maintained set
  /// </summary>
  /// <remarks>
  /// This method does not guarantee that the specified ExposeViewWindow is immediately removed.
  /// Actual removal will take place at some point later, depending on what the user is doing and
  /// how long it's going to take any shutdown animations to run.
  ///
  /// Upon return of this call, the specified ExposeViewWindow will not be enumerable from the
  /// ExposeView proper.
  /// </remarks>
  void RemoveExposeWindow(const std::shared_ptr<ExposeViewWindow>& wnd);

  /// <summary>
  /// </summary>
  void StartView();
  void CloseView();

  /// <summary>
  /// Recovers a window from the specified abstract coordinates
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(double x, double y) const;
};

