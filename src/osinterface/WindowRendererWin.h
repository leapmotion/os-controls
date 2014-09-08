#pragma once
#include "WindowRenderer.h"

class ImagePrimitive;

class WindowRendererWin:
  public WindowRenderer
{
public:
  std::shared_ptr<ImagePrimitive> Render(HWND hwnd, std::shared_ptr<ImagePrimitive> img);
};

