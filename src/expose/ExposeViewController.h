#pragma once
#include "osinterface/OSWindow.h"
#include "osinterface/OSWindowEvent.h"
#include "ExposeViewWindow.h"
#include "ExposeViewEvents.h"
#include <map>

class ExposeView;

class ExposeViewController :
public OSWindowEvent,
public ExposeViewEvents{
public:
  ExposeViewController();
  virtual~ExposeViewController();
  
  void OnCreate(const OSWindow& window) override;
  void OnDestroy(const OSWindow& window) override;
  
  void onSelectionMade(std::shared_ptr<const OSWindow> window) override;
  
private:
  bool windowExists(uint64_t uniqueId);
  
  std::map<std::shared_ptr<const OSWindow>, std::shared_ptr<ExposeViewWindow>> m_windows;
  AutoRequired<ExposeView> m_exposeView;
};