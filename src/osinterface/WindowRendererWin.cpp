#include "stdafx.h"
#include "WindowRendererWin.h"
#include "OSWindowWin.h"
#include "DwmpApi.h"
#include <thread>

int APIENTRY DwmpDxGetWindowSharedSurface(HWND hWnd, LUID adapterLuid, LUID someLuid, DWORD *pD3DFormat, HANDLE *pSharedHandle, unsigned __int64 *arg7);
int	APIENTRY D3DKMTOpenAdapterFromGdiDisplayName(IN OUT D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME *pData);
int	APIENTRY D3DKMTCloseAdapter(IN CONST D3DKMT_CLOSEADAPTER *pData);

HMODULE hgdi = LoadLibrary("gdi32");
auto g_D3DKMTOpenAdapterFromGdiDisplayName = (decltype(D3DKMTOpenAdapterFromGdiDisplayName)*) GetProcAddress(hgdi, "D3DKMTOpenAdapterFromGdiDisplayName");
auto g_D3DKMTCloseAdapter = (decltype(D3DKMTCloseAdapter)*) GetProcAddress(hgdi, "D3DKMTCloseAdapter");

HMODULE hdwm = LoadLibrary("dwmapi");
auto g_DwmpDxGetWindowSharedSurface = (decltype(DwmpDxGetWindowSharedSurface)*)GetProcAddress(hdwm, (LPCSTR) 100);

void GetWindowSharedSurfaceHandle(HWND hWnd, HANDLE *sharedHandle, DWORD *d3dFormat)
{
  HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

  MONITORINFOEX monitorInfo;
  monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hMonitor, &monitorInfo);

  D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openDev;
  mbstowcs(openDev.DeviceName, monitorInfo.szDevice, sizeof(monitorInfo.szDevice));

  g_D3DKMTOpenAdapterFromGdiDisplayName(&openDev);

  {
    D3DKMT_CLOSEADAPTER closeDev;
    closeDev.hAdapter = openDev.hAdapter;
    g_D3DKMTCloseAdapter(&closeDev);
  }

  LUID nullLuid = {0, 0};
  uint64_t unknown = 0;
  g_DwmpDxGetWindowSharedSurface(hWnd, openDev.AdapterLuid, nullLuid, d3dFormat, sharedHandle, &unknown);
}

WindowRendererWin::WindowRendererWin(void) {}
WindowRendererWin::~WindowRendererWin(void) {}

WindowRenderer* WindowRenderer::New(void) {
  return new WindowRendererWin;
}

void WindowRendererWin::Render(OSWindowWin& osWindow, std::vector<unsigned char>& mem) {
  HANDLE sharedHandle = nullptr;
  DWORD d3dFormat = 0;
  GetWindowSharedSurfaceHandle(osWindow.hwnd, &sharedHandle, &d3dFormat);
}
