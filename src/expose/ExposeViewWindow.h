#pragma once
#include "expose/ExposeView.h"
#include "graphics/Renderable.h"
#include "utility/lockable_property.h"
#include "Animation.h"
#include "DropShadow.h"

class OSWindow;
struct RenderFrame;

class ExposeViewWindow:
  public Renderable
{
public:
  ExposeViewWindow(OSWindow& osWindow);
  ~ExposeViewWindow(void);

  // Flag, set if the view can be automatically laid out.  If this flag is cleared,
  // ExposeView should not attempt to lay out the window.
  lockable_property<void> m_layoutLocked;

  // The underlying OS window
  const std::shared_ptr<OSWindow> m_osWindow;

  std::shared_ptr<ImagePrimitive>& GetTexture() { return m_texture; }
  const std::shared_ptr<ImagePrimitive>& GetTexture() const { return m_texture; }

  void SetOpeningPosition();
  void SetClosingPosition();
  Vector2 GetOSPosition() const;
  Vector2 GetOSSize() const;

  float GetScale() const {
    return m_scale.Value();
  }

  static const double VIEW_ANIMATION_TIME;

  // Smooth animations for opacity and position
  Smoothed<float> m_opacity;
  Animated<Vector3> m_position;
  Smoothed<float> m_scale;
  Smoothed<Vector3> m_grabDelta;
  Smoothed<Vector3> m_forceDelta;

  // Smooth animations for hover and activation
  Smoothed<float> m_hover;
  Smoothed<float> m_activation;
  Smoothed<float> m_selection;

  Vector3 m_prevPosition;

private:
  // Texture for this window
  std::shared_ptr<ImagePrimitive> m_texture;

  // Drop shadow
  std::shared_ptr<DropShadow> m_dropShadow;

  // Highlight
  std::shared_ptr<RectanglePrim> m_highlight;

  bool m_closing;

public:
  /// <summary>
  /// Causes this window to attempt to update its texture from the underlying OSWindow
  /// </summary>
  /// <remarks>
  /// Because this method makes OpenGL calls, it can only be safely called from the OpeNGL thread
  /// </remarks>
  void UpdateTexture(void);

  // RenderEngineNode overrides
  void Render(const RenderFrame& frame) const override;
};
