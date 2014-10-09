#include "CompositionEngineWin.h"

#include <d3d11.h>
#include <dcomp.h>

CompositionEngine* CompositionEngine::New() {
  return new CompositionEngineWin();
}

CompositionEngineWin::CompositionEngineWin()
{
  D3D_FEATURE_LEVEL supportedLevel;

  ::D3D11CreateDevice(nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    0,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &m_D3D11Device,
    &supportedLevel,
    nullptr
  );

  m_D3D11Device->QueryInterface(&m_DXGIDevice);
  DCompositionCreateDevice(m_DXGIDevice, __uuidof(IDCompositionDevice), reinterpret_cast<void**>(&m_DCompDevice));

}


CompositionEngineWin::~CompositionEngineWin()
{
}
