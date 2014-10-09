#pragma once
#include "CompositionEngine.h"

class ID3D11Device;
class IDXGIDevice;
class IDCompositionDevice;

class CompositionEngineWin :
  public CompositionEngine
{
public:
  CompositionEngineWin();
  virtual ~CompositionEngineWin();

private:
  ID3D11Device* m_D3D11Device;
  IDXGIDevice* m_DXGIDevice;
  IDCompositionDevice* m_DCompDevice;
};

