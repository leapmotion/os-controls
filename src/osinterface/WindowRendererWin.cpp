#include "stdafx.h"
#include "WindowRendererWin.h"
#include "DwmpApi.h"
#include "utility/HandleUtilitiesWin.h"
#include <thread>
#include <Primitives.h>

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

#ifndef NT_ERROR
#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)
#endif

HRESULT APIENTRY DwmpDxGetWindowSharedSurface(HWND hWnd, LUID adapterLuid, LUID someLuid, DXGI_FORMAT *pD3DFormat, HANDLE* pSharedHandle, unsigned __int64 *arg7);
NTSTATUS APIENTRY D3DKMTOpenAdapterFromGdiDisplayName(IN OUT D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME *pData);
NTSTATUS APIENTRY D3DKMTCloseAdapter(IN CONST D3DKMT_CLOSEADAPTER *pData);

HMODULE hgdi = LoadLibrary("gdi32");
auto g_D3DKMTOpenAdapterFromGdiDisplayName = (decltype(D3DKMTOpenAdapterFromGdiDisplayName)*) GetProcAddress(hgdi, "D3DKMTOpenAdapterFromGdiDisplayName");
auto g_D3DKMTCloseAdapter = (decltype(D3DKMTCloseAdapter)*) GetProcAddress(hgdi, "D3DKMTCloseAdapter");

HMODULE hdwm = LoadLibrary("dwmapi");
auto g_DwmpDxGetWindowSharedSurface = (decltype(DwmpDxGetWindowSharedSurface)*)GetProcAddress(hdwm, (LPCSTR) 100);

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

HRESULT GetWindowSharedSurfaceHandle(HWND hWnd, HANDLE& sharedHandle, DXGI_FORMAT& d3dFormat)
{
  HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

  MONITORINFOEX monitorInfo;
  monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hMonitor, &monitorInfo);

  D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openDev;

  const char* szDevice = monitorInfo.szDevice;
  size_t nConverted;
  mbstate_t mbState;
  mbsrtowcs_s(&nConverted, openDev.DeviceName, &szDevice, sizeof(monitorInfo.szDevice), &mbState);

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
  LARGE_INTEGER unknown = {};
  g_DwmpDxGetWindowSharedSurface(hWnd, openDev.AdapterLuid, nullLuid, &d3dFormat, &sharedHandle, (unsigned long long*)&unknown.QuadPart);

  return S_OK;
}

WindowRenderer* WindowRenderer::New(void) {
  return new WindowRendererWin;
}

std::shared_ptr<ImagePrimitive> WindowRendererWin::Render(HWND hwnd, std::shared_ptr<ImagePrimitive> img) {
  struct {
    HANDLE sharedHandle;
    void* forbidden;
  } space;
  space.forbidden = nullptr;

  HANDLE hRsrc;
  DXGI_FORMAT format;
  HRESULT hr = GetWindowSharedSurfaceHandle(hwnd, hRsrc, format);
  if(FAILED(hr))
    return false;

  CComPtr<IDXGISurface> pSurface;
  hr = m_device->OpenSharedResource(hRsrc, __uuidof(*pSurface), (void**) &pSurface);

  return img;
}
