#pragma once

class OSWindowEvent
{
public:
  virtual ~OSWindowEvent(void) {}

  virtual void OnCreate(const OSWindow& window) {}
  virtual void OnDestroy(const OSWindow& window) {}
};

