#pragma once
#include "graphics/Renderable.h"

/// <summary>
/// Represents a way to render OS windows.
/// </summary>
class OSWindowRenderer:
  public Renderable
{
public:
  OSWindowRenderer(void);
  ~OSWindowRenderer(void);
};

