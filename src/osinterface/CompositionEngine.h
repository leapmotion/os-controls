#pragma once

#include "OSWindowHandle.h"
#include "autowiring/Object.h"

class ComposedView;
class ComposedDisplay;

class CompositionEngine :
  public Object
{
public:
  virtual ~CompositionEngine() {}

  static CompositionEngine* New(void);
  
  virtual ComposedView* CreateView() = 0;
  virtual ComposedDisplay* CreateDisplay(WindowHandle handle) = 0;

  virtual void CommitChanges() = 0;
  virtual bool CommitRequired() const = 0;
};

class ComposedView :
  public Object
{
public:
  virtual ~ComposedView() {}
  virtual void SetContent(const WindowHandle& window) = 0;
  virtual void AddChild(ComposedView* view) = 0;

  virtual void SetOffset(float x, float y) = 0;
  virtual void SetRotation(float centerX, float centerY, float degrees) = 0;
  virtual void SetScale(float centerX, float centerY, float scaleX, float scaleY) = 0;
};

class ComposedDisplay :
  public Object
{
public:
  virtual void SetView(ComposedView* view) = 0;
};