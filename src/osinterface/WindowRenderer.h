#pragma once
#include <autowiring/ContextMember.h>
#include <vector>

class OSWindow;

/// <summary>
/// OS-specific class to render a window to a desktop
/// </summary>
class WindowRenderer:
  public ContextMember
{
public:
  WindowRenderer(void);
  ~WindowRenderer(void);

  static WindowRenderer* New(void);
};

