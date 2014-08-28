#pragma once
#include "RenderEngine.h"
#include "RenderEngineNode.h"
#include "Primitives.h"
#include <Animation.h>
#include <vector>
#include <tuple>


class ExposeViewWindow;

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
  
  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;
  
  void StartView();
  void CloseView();

  // Given the cursor position, layout the windows properly.
  // This is impacted by startGrab and endGrab
  void UpdateLayout(uint32_t cursorX, uint32_t cursorY);
  
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

  /// Recovers a window from the specified abstract coordinates
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(double x, double y) const;
  
private:
  // Find the given window in the window list and move
  // to the end of the vector such that it will render on top.
  void moveWindowToTop(std::shared_ptr<RectanglePrim> window);
  
  // Convert a radian angle and a pixel distance to a point.
  // Returns a tuple x,y
  std::tuple<double, double> radialCoordsToPoint();

  
  Autowired<RootRenderEngineNode> m_rootNode; //Root node in the render tree
  
  bool m_handIsGrabbing; //Flag to know if the hand is currently grabbing
  
  Animated<float> m_opacity;
  
  //Each of the window images.
  std::vector<std::shared_ptr<RectanglePrim>> m_windows;
};

