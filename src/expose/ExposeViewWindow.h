#pragma once
#include "ExposeView.h"
#include "graphics/RenderEngineNode.h"
#include "utility/lockable_property.h"

class OSWindow;

class ExposeViewWindow:
  public RenderEngineNode
{
public:
  ExposeViewWindow(OSWindow& osWindow);
  ~ExposeViewWindow(void);

  // Current activation level, some number in the range [0...1]
  lockable_property<float> m_activation;

  // Flag, set if the view can be automatically laid out.  If this flag is cleared,
  // ExposeView should not attempt to lay out the window.
  lockable_property<void> m_layoutLocked;

  // The underlying OS window
  const std::shared_ptr<OSWindow> m_osWindow;

private:
  // Texture for this window
  std::unique_ptr<ImagePrimitive> m_texture;

public:
  /// <summary>
  /// Causes this window to attempt to update its texture from the underlying OSWindow
  /// </summary>
  /// <remarks>
  /// Because this method makes OpenGL calls, it can only be safely called from the OpeNGL thread
  /// </remarks>
  void UpdateTexture(void);

  // RenderEngineNode overrides
  void Render(const RenderFrame& frame) const;
};
