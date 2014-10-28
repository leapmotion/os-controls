#include "CompositionEngineWin.h"

#include <d3d11.h>
#include <dcomp.h>
#include <stdexcept>

CompositionEngine* CompositionEngine::New() {
  return new CompositionEngineWin();
}

ComposedView* CompositionEngineWin::CreateView() {
  return new ComposedViewWin(this);
}

ComposedDisplay* CompositionEngineWin::CreateDisplay(WindowHandle handle){
  return new ComposedDisplayWin(this, handle);
}

CompositionEngineWin::CompositionEngineWin() :
m_commitRequired(false)
{
  D3D_FEATURE_LEVEL supportedLevel;

  HRESULT hr = ::D3D11CreateDevice(nullptr,
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

  if (!SUCCEEDED(hr))
    throw std::runtime_error("Failed to create D3D11 Device - " + hr);

  hr = m_D3D11Device->QueryInterface(&m_DXGIDevice);
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Failed to obtain DXGI Device - " + hr);

  hr = DCompositionCreateDevice(m_DXGIDevice, __uuidof(IDCompositionDevice), reinterpret_cast<void**>(&m_DCompDevice));
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Failed to create DirectComposition device - " + hr);
}

CompositionEngineWin::~CompositionEngineWin() {
}

void CompositionEngineWin::CommitChanges(){
  m_DCompDevice->Commit();
}

ComposedViewWin::ComposedViewWin(CompositionEngineWin *engine) :
m_device(engine),
m_visual(nullptr)
{
  HRESULT hr = m_device->m_DCompDevice->CreateVisual(&m_visual);
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Error creating DirectComposition Visual - " + hr);

  hr = m_device->m_DCompDevice->CreateRotateTransform(&m_rotateTransform);
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Error creating RotationTransform");

  hr = m_device->m_DCompDevice->CreateScaleTransform(&m_scaleTransform);
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Error creating ScaleTransform");

  IDCompositionTransform* transforms[2] = { m_rotateTransform, m_scaleTransform };

  //this relies on the transforms being contiguous in memory
  hr = m_device->m_DCompDevice->CreateTransformGroup(transforms, 2, &m_transformGroup);

  m_visual->SetTransform(m_transformGroup);
}

ComposedViewWin::~ComposedViewWin() {
}

void ComposedViewWin::SetContent(const WindowHandle& window){
  IUnknown* pSurface = nullptr;

  LONG style = ::GetWindowLong(window, GWL_EXSTYLE);
  if ((style & WS_EX_LAYERED) == 0){
    ::SetWindowLong(window, GWL_EXSTYLE, style | WS_EX_LAYERED);
    ::SetLayeredWindowAttributes(window, RGB(0, 0, 0), 255, LWA_ALPHA);
  }

  HRESULT hr = m_device->m_DCompDevice->CreateSurfaceFromHwnd(window,&pSurface);

  if (!SUCCEEDED(hr))
    throw std::runtime_error("Failed to create surface from HWND - " + hr);

  m_visual->SetContent(pSurface);

  pSurface->Release();
  m_device->m_commitRequired = true;
}

void ComposedViewWin::AddChild(ComposedView* visual) {
  ComposedViewWin* winVis = static_cast<ComposedViewWin*>(visual);
  m_visual->AddVisual(winVis->m_visual, TRUE, nullptr);
  m_children.push_back(winVis);

  m_device->m_commitRequired = true;
}

void ComposedViewWin::SetOffset(float x, float y){
  m_visual->SetOffsetX(x);
  m_visual->SetOffsetY(y);

  m_device->m_commitRequired = true;
}

void ComposedViewWin::SetRotation(float centerX, float centerY, float degrees) {
  m_rotateTransform->SetCenterX(centerX);
  m_rotateTransform->SetCenterY(centerY);
  m_rotateTransform->SetAngle(degrees);

  m_device->m_commitRequired = true;
}

void ComposedViewWin::SetScale(float centerX, float centerY, float scaleX, float scaleY) {
  m_scaleTransform->SetCenterX(centerX);
  m_scaleTransform->SetCenterY(centerY);
  m_scaleTransform->SetScaleX(scaleX);
  m_scaleTransform->SetScaleY(scaleY);

  m_device->m_commitRequired = true;
}

void ComposedViewWin::SetClip(float x, float y, float width, float height) {
  D2D_RECT_F clipRect{ x, y , x + width, y + height };
  m_visual->SetClip(clipRect);

  m_device->m_commitRequired = true;
}


ComposedDisplayWin::ComposedDisplayWin(CompositionEngineWin* engine, WindowHandle window) :
  m_target(nullptr),
  m_engine(engine)
{
  HRESULT hr = m_engine->m_DCompDevice->CreateTargetForHwnd(window, TRUE, &m_target);
  if (!SUCCEEDED(hr))
    throw std::runtime_error("Failed to create target from HWND - " + hr);
}

void ComposedDisplayWin::SetView(ComposedView* visual){
  ComposedViewWin* winVis = static_cast<ComposedViewWin*>(visual);
  m_target->SetRoot(winVis->m_visual);
}