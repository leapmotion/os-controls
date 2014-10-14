#pragma once

#include "OSWindowHandle.h"

class CompositionEngine
{
public:
  virtual ~CompositionEngine() {}

  static CompositionEngine* New(void);

  virtual void CommitChanges() = 0;
  virtual bool CommitRequired() const = 0;
};

class ComposedView
{
public:

  static ComposedView* New(CompositionEngine* engine);
  
  virtual ~ComposedView() {}
  virtual void SetContent(const WindowHandle& window) = 0;
  virtual void AddChild(ComposedView* view) = 0;

  virtual void SetOffset(float x, float y) = 0;
  virtual void SetRotation(float centerX, float centerY, float degrees) = 0;
  virtual void SetScale(float centerX, float centerY, float scaleX, float scaleY) = 0;
};

class ComposedDisplay
{
public:
  static ComposedDisplay* New(CompositionEngine* engine, WindowHandle* handle);

  virtual void SetView(ComposedView* view) = 0;
};