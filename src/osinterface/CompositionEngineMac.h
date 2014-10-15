#pragma once
#include "CompositionEngine.h"

class CompositionEngineMac
  : public CompositionEngine
{
public:
  CompositionEngineMac();
  virtual ~CompositionEngineMac() {}

  ComposedView* CreateView() override;
  ComposedDisplay* CreateDisplay(WindowHandle handle) override;

  void CommitChanges() override;
  bool CommitRequired() const override;
};

class ComposedViewMac :
  public ComposedView
{
  virtual void SetContent(const WindowHandle& window) override {}
  virtual void AddChild(ComposedView* view) override {}

  virtual void SetOffset(float x, float y) override {}
  virtual void SetRotation(float centerX, float centerY, float degrees) override {}
  virtual void SetScale(float centerX, float centerY, float scaleX, float scaleY) override {}
};

class ComposedDisplayMac :
  public ComposedDisplay
{
  void SetView(ComposedView* view) override {}
};
