#include "stdafx.h"
#include "WindowRendererWin.h"
#include "OSWindowWin.h"
#include <D3D11.h>
#include <thread>

WindowRendererWin::WindowRendererWin(void)
{
  HRESULT hr;

  D3D_FEATURE_LEVEL featureLevel;
  hr = D3D11CreateDevice(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    0,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &m_device,
    &featureLevel,
    &m_context
  );
  if(FAILED(hr))
    throw std::runtime_error("Failed to create a hardware-accelerated device");

  m_dxgiDevice = m_device;
  if(!m_dxgiDevice)
    throw std::runtime_error("Failed to obtain a DXGI interface from a DX11 device");

  hr = DCompositionCreateDevice2(m_dxgiDevice, __uuidof(*m_compDevice), (void**) &m_compDevice);
  if(FAILED(hr))
    throw std::runtime_error("Failed to create a DirectX composition device");
}

WindowRendererWin::~WindowRendererWin(void)
{
}

WindowRenderer* WindowRenderer::New(void) {
  return new WindowRendererWin;
}

void WindowRendererWin::Render(OSWindowWin& osWindow, std::vector<unsigned char>& mem) {
  CComPtr<IUnknown> iSurface;
  HRESULT hr = m_compDevice->CreateSurfaceFromHwnd(osWindow.hwnd, &iSurface);
  if(FAILED(hr))
    return;

  CComQIPtr<IDCompositionSurface> surface = iSurface;
  //surface->
}
