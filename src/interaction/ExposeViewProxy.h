#pragma once
#include "uievents/OSCDomain.h"
#include "graphics/ExposeView.h"
#include "HandLocationRecognizer.h"

class ExposeViewAccessManager;

class ExposeViewProxy
{
public:
  ExposeViewProxy();
  ~ExposeViewProxy();

  void AutoFilter(OSCState appState, const HandLocation& handLocation);

private:
  enum class State {
    INACTIVE,
    AWAITING_LOCK,
    ACTIVE,
  };
  
  State m_state;
  
  std::shared_ptr<ExposeView> m_exposeView;
  
  Autowired<ExposeViewAccessManager> m_exposeViewAccessManager;
};

