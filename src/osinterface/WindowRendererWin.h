#pragma once
#include "WindowRenderer.h"

class OSWindowWin;

class WindowRendererWin:
  public WindowRenderer
{
public:
  WindowRendererWin(void);
  ~WindowRendererWin(void);


public:
  void Render(OSWindowWin& osWindow, std::vector<unsigned char>& mem);
};

