#pragma once
#include "WindowRenderer.h"

class WindowRendererWin:
  public WindowRenderer
{
public:
  WindowRendererWin(void);
  ~WindowRendererWin(void);

private:

public:
  // WindowRenderer overrides:
  void Render(OSWindow& osWindow, std::vector<unsigned char>& mem) override;
};

