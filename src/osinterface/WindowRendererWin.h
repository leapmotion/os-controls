#pragma once
#include "WindowRenderer.h"
#include <Dcomp.h>

class OSWindowWin;
struct ID3D11Device;
struct ID3D11DeviceContext;

class WindowRendererWin:
  public WindowRenderer
{
public:
  WindowRendererWin(void);
  ~WindowRendererWin(void);

private:
  CComPtr<IDXGIFactory> m_dxgiFactory;
  CComPtr<ID3D11Device> m_device;
  CComPtr<ID3D11DeviceContext> m_context;
  CComQIPtr<IDXGIDevice> m_dxgiDevice;
  CComPtr<IDCompositionDesktopDevice> m_compDevice;

public:
  void Render(OSWindowWin& osWindow, std::vector<unsigned char>& mem);
};

