#pragma once
#include "osinterface/OSWindow.h"
#include "osinterface/OSWindowEvent.h"
#include <map>

struct ApplicationGroup {
  int32_t uniqueId;
  std::map<int32_t, OSWindow&> windows;
};

class ExposeViewController :
public OSWindowEvent {
public:
  ExposeViewController();
  virtual~ExposeViewController();
  
  void OnCreate(const OSWindow& window) override;
  void OnDestroy(const OSWindow& window) override;
  void UpdateViewModel();
  
private:
  bool appExists(int32_t uniqueId);
  std::map<int32_t, ApplicationGroup&> applications;
};