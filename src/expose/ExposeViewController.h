#pragma once
#include "osinterface/OSWindow.h"
#include "osinterface/OSWindowEvent.h"
#include <map>

class ExposeViewController :
public OSWindowEvent {
public:
  ExposeViewController();
  virtual~ExposeViewController();
  
  void OnCreate(const OSWindow& window) override;
  void OnDestroy(const OSWindow& window) override;
  
private:
  bool windowExists(int32_t uniqueId);
  std::map<ExposeViewWindow, OSWindow&> windows;
};