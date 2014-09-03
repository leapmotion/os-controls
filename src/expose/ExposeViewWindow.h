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
  RectanglePrim m_texture;

public:
  // RenderEngineNode overrides
  void Render(const RenderFrame& frame) const;
};
