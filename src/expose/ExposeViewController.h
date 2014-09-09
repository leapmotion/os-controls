#pragma once
#include "ExposeViewWindow.h"
#include "ExposeViewEvents.h"
#include "osinterface/OSWindowEvent.h"
#include <map>

class ExposeView;
class OSWindow;

class ExposeViewController:
  public OSWindowEvent,
  public ExposeViewEvents
{
public:
  ExposeViewController();
  virtual ~ExposeViewController() {}
  
  void OnCreate(OSWindow& window) override;
  void OnDestroy(OSWindow& window) override;
  void OnResize(OSWindow& window) override;
  
  void onWindowSelected(ExposeViewWindow& wnd) override;
  
private:
  AutoRequired<ExposeView> m_exposeView;

  // Dictionary used to detect changes to OS state
  std::unordered_map<std::shared_ptr<OSWindow>, std::shared_ptr<ExposeViewWindow>> m_windows;
};
