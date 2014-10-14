#pragma once
#include "CompositionEngine.h"
#include <autowiring/Autowired.h>
#include <vector>

struct ID3D11Device;
struct IDXGIDevice;
struct IDCompositionDevice;
struct IDCompositionTarget;
struct IDCompositionSurface;
struct IDCompositionVisual;
struct IDCompositionRotateTransform;
struct IDCompositionScaleTransform;
struct IDCompositionTransform;

class CompositionEngineWin :
  public CompositionEngine
{
public:
  CompositionEngineWin();
  virtual ~CompositionEngineWin();

  void CommitChanges() override;
  bool CommitRequired() const override{ return m_commitRequired; }

private:
  bool m_commitRequired;

  ID3D11Device* m_D3D11Device;
  IDXGIDevice* m_DXGIDevice;
  IDCompositionDevice* m_DCompDevice;
  friend class ComposedViewWin;
  friend class ComposedDisplayWin;
};

class ComposedDisplayWin :
  public ComposedDisplay
{
public:
  ComposedDisplayWin(CompositionEngineWin* engine, WindowHandle window);

  void SetView(ComposedView* visual) override;

private:
  IDCompositionTarget* m_target;
  CompositionEngineWin* m_engine;

  std::vector<ComposedView*> m_children;
};

class ComposedViewWin :
  public ComposedView
{
public:
  ComposedViewWin(CompositionEngineWin* device);
  virtual ~ComposedViewWin();

  void SetContent(const WindowHandle& window) override;

  void AddChild(ComposedView* visual) override;

  void SetOffset(float x, float y) override;
  void SetRotation(float centerX, float centerY, float degrees) override;
  void SetScale(float centerX, float centerY, float scaleX, float scaleY) override;

private:
  friend class ComposedDisplayWin;
  CompositionEngineWin* m_device;

  IDCompositionVisual* m_visual;

  IDCompositionRotateTransform* m_rotateTransform;
  IDCompositionScaleTransform* m_scaleTransform;
  IDCompositionTransform* m_transformGroup;

  std::vector<ComposedViewWin*> m_children;
};
