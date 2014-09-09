#pragma once
#include "Primitives.h"
#include "graphics/Renderable.h"
#include <autowiring/DispatchQueue.h>
#include <Animation.h>
#include "interaction/HandDataCombiner.h"
#include "expose/ExposeViewEvents.h"
#include <vector>
#include <tuple>

class ExposeViewWindow;
class OSWindow;
class RenderEngine;
class SVGPrimitive;

/// <summary>
/// Implements expose view
/// </summary>
class ExposeView:
  public ContextMember,
  public Renderable,
  DispatchQueue
{
public:
  ExposeView(void);
  ~ExposeView(void);
  void AutoInit();
  
private:
  //Root node in the render tree
  Autowired<RenderEngine> m_rootNode;
  
  //Events to send to controller
  AutoFired<ExposeViewEvents> m_exposeViewEvents;

  // Alpha masking value for the entire view
  Animated<float> m_alphaMask;
  
  // All windows currently known to this view:
  std::unordered_set<std::shared_ptr<ExposeViewWindow>> m_windows;

  // Windows represented in order:
  Renderable::ZOrderList m_zorder;

  // Background Overlay Rectangle
  std::shared_ptr<RectanglePrim> m_backgroundRect;


  // Hand data
  HandData m_handData;

private:
  struct Force {
    Force(const Vector3& position, float strength, ExposeViewWindow* wnd, float maxDist) :
      m_position(position),
      m_strength(strength),
      m_window(wnd),
      m_maxDist(maxDist)
    {}
    Vector3 ForceAt(const Vector3& position) const {
      const Vector3 diff = position - m_position;
      const double dist = diff.norm();
      const double distMult = 1.0 - SmootherStep(std::min(1.0, dist/m_maxDist));
      return m_strength * distMult * diff / dist;
    }
    Vector3 m_position;
    float m_strength;
    ExposeViewWindow* m_window;
    float m_maxDist;
  };

  typedef std::vector<Force, Eigen::aligned_allocator<Force> > ForceVector;

  ForceVector m_forces;
  double m_layoutRadius;
  double m_selectionRadius;
  Vector2 m_viewCenter;

  std::shared_ptr<Disk> m_selectionRegion;
  std::shared_ptr<PartialDisk> m_selectionOutline;

  /// <summary>
  /// Evolves the layout by one step
  /// </summary>
  void updateLayout(std::chrono::duration<double> dt);
  
  void updateActivations(std::chrono::duration<double> dt);

  void updateForces(std::chrono::duration<double> dt);

  // Send commend to controller to focus the given window.
  void focusWindow(ExposeViewWindow& windowToFocus);

  void updateWindowTextures();
  void updateWindowTexturesRoundRobin();
  
  // Convert a radian angle and a pixel distance to a point.
  // Returns a tuple x,y
  Vector2 radialCoordsToPoint(double angle, double distance);

public:
  // RenderEngineNode overrides:
  void AnimationUpdate(const RenderFrame& frame) override;
  void Render(const RenderFrame& frame) const override;

  /// <returns>
  /// True if the ExposeView is presently visible to the user
  /// </returns>
  bool IsVisible(void) const { return 0.001f < m_alphaMask.Current(); }

  void SetHandData(const HandData& handData) { m_handData = handData; }

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
  /// Update the texture of the specified expose window
  /// </summary>
  void UpdateExposeWindow(const std::shared_ptr<ExposeViewWindow>& wnd);

  /// <summary>
  /// </summary>
  void StartView();
  void CloseView();

  /// <summary>
  /// Recovers a window from the specified abstract coordinates
  /// </summary>
  std::shared_ptr<ExposeViewWindow> WindowFromPoint(double x, double y) const;
};

