#pragma once

class OSWindowEvent
{
public:
  virtual ~OSWindowEvent(void) {}

  virtual void OnCreate(OSWindow& window) {}
  virtual void OnDestroy(OSWindow& window) {}
};

