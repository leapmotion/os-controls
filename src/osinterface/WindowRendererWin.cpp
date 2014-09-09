#include "stdafx.h"
#include "WindowRendererWin.h"
#include "DwmpApi.h"
#include "utility/HandleUtilitiesWin.h"
#include <DirectXTex/DirectXTex.h>
#include <Primitives.h>
#include <iomanip>

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#ifndef NT_ERROR
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)
#endif

NTSTATUS APIENTRY D3DKMTOpenAdapterFromGdiDisplayName(IN OUT D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME *pData);
NTSTATUS APIENTRY D3DKMTCloseAdapter(IN CONST D3DKMT_CLOSEADAPTER *pData);
HRESULT APIENTRY DwmpDxGetWindowSharedSurface(HWND hWnd, LUID adapterLuid, LUID someLuid, DXGI_FORMAT *pD3DFormat, HANDLE* pSharedHandle, unsigned __int64 *arg7);
HRESULT APIENTRY DwmpUpdateWindowSharedSurface(HANDLE sharedHandle, int, int, int, HMONITOR hMonitor, void* unknown);

HMODULE hgdi = LoadLibrary("gdi32");
auto g_D3DKMTOpenAdapterFromGdiDisplayName = (decltype(D3DKMTOpenAdapterFromGdiDisplayName)*) GetProcAddress(hgdi, "D3DKMTOpenAdapterFromGdiDisplayName");
auto g_D3DKMTCloseAdapter = (decltype(D3DKMTCloseAdapter)*) GetProcAddress(hgdi, "D3DKMTCloseAdapter");

HMODULE hdwm = LoadLibrary("dwmapi");
auto g_DwmpDxGetWindowSharedSurface = (decltype(DwmpDxGetWindowSharedSurface)*)GetProcAddress(hdwm, (LPCSTR) 100);
auto g_DwmpUpdateWindowSharedSurface = (decltype(DwmpUpdateWindowSharedSurface)*) GetProcAddress(hdwm, (LPCSTR) 101);

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
}

HRESULT WindowRendererWin::CreateWindowSharedSurface(HWND hWnd, CComPtr<ID3D11Resource>& resource)
{
  HRESULT hr;

  MONITORINFOEX monitorInfo;
  {
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    monitorInfo.cbSize = sizeof(monitorInfo);
	  GetMonitorInfo(hMonitor, &monitorInfo);
  }

  D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openDev;

  {
    const char* szDevice = monitorInfo.szDevice;
    size_t nConverted;
    mbstate_t mbState;
    mbsrtowcs_s(&nConverted, openDev.DeviceName, &szDevice, sizeof(monitorInfo.szDevice), &mbState);
  }

  auto status = g_D3DKMTOpenAdapterFromGdiDisplayName(&openDev);
  if(NT_ERROR(status))
    return HRESULT_FROM_NT(status);

  {
    D3DKMT_CLOSEADAPTER closeDev;
    closeDev.hAdapter = openDev.hAdapter;
    status = g_D3DKMTCloseAdapter(&closeDev);
    if(NT_ERROR(status))
      return HRESULT_FROM_NT(status);
  }

  LUID nullLuid = {0, 0};
  unsigned long long unknown;

  DXGI_FORMAT d3dFormat;
  HANDLE sharedHandle;
  hr = g_DwmpDxGetWindowSharedSurface(hWnd, openDev.AdapterLuid, nullLuid, &d3dFormat, &sharedHandle, &unknown);
  if(FAILED(hr))
    return hr;

  hr = m_device->OpenSharedResource(sharedHandle, __uuidof(*resource), (void**) &resource);
  if(FAILED(hr))
    return hr;

  CComQIPtr<ID3D11Texture2D> tex = resource;

  hr = g_DwmpUpdateWindowSharedSurface(sharedHandle, 0, 0, 0, nullptr, nullptr);
  if(FAILED(hr))
    return hr;

  static int v = 0;
  std::wstringstream ss;
  ss << "C:\\temp\\rs" << std::setfill(L'0') << std::setw(2) << v << ".tga";
  v++;

  DirectX::ScratchImage si;
  DirectX::CaptureTexture(m_device, m_context, tex, si);
  DirectX::SaveToTGAFile(*si.GetImage(0, 0, 0), ss.str().c_str());

  return S_OK;
}

WindowRenderer* WindowRenderer::New(void) {
  return new WindowRendererWin;
}

std::shared_ptr<ImagePrimitive> WindowRendererWin::Render(HWND hwnd, std::shared_ptr<ImagePrimitive> img) {
  CComPtr<ID3D11Resource> resource;
  CreateWindowSharedSurface(hwnd, resource);
  return img;
}
