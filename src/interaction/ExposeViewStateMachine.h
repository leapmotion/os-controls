#pragma once
#include "expose/ExposeView.h"
#include "uievents/OSCDomain.h"
#include "HandLocationRecognizer.h"

class ExposeViewAccessManager;

class ExposeViewStateMachine
{
public:
  ExposeViewStateMachine();
  ~ExposeViewStateMachine();

  void AutoFilter(OSCState appState, const HandData& handData);

private:
  
  enum class State {
    INACTIVE,
    AWAITING_LOCK,
    ACTIVE,
  };
  
  void applyUserInput(const HandLocation& handLocation);
  
  State m_state;
  
  std::shared_ptr<ExposeView> m_exposeView;
  
  Autowired<ExposeViewAccessManager> m_exposeViewAccessManager;
};

