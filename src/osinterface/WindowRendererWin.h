#pragma once
#include "WindowRenderer.h"
#include <D3D11.h>

class ImagePrimitive;

class WindowRendererWin:
  public WindowRenderer
{
public:
  WindowRendererWin(void);

private:
  CComPtr<IDXGIFactory> m_dxgiFactory;
  CComPtr<ID3D11Device> m_device;
  CComQIPtr<IDXGIDevice> m_dxgiDevice;
  CComPtr<ID3D11DeviceContext> m_context;

public:
  std::shared_ptr<ImagePrimitive> Render(HWND hwnd, std::shared_ptr<ImagePrimitive> img);
};

