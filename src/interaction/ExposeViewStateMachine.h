#pragma once
#include "expose/ExposeView.h"
#include "uievents/OSCDomain.h"
#include "HandLocationRecognizer.h"
#include "expose/ExposeViewWindow.h"

class ExposeViewAccessManager;

class ExposeViewStateMachine :
  public ExposeViewEvents,
  public ContextMember

{
public:
  ExposeViewStateMachine();
  ~ExposeViewStateMachine();

  void AutoFilter(OSCState appState, const HandData& handData);
  void onWindowSelected(ExposeViewWindow& osWindow) override;

private:
  
  enum class State {
    INACTIVE,
    AWAITING_LOCK,
    ACTIVE,
    COMPLETE
  };
  
  void applyUserInput(const HandLocation& handLocation);
  
  State m_state;
  
  std::shared_ptr<ExposeView> m_exposeView;
  
  Autowired<ExposeViewAccessManager> m_exposeViewAccessManager;

public:
  bool IsComplete() const { return m_state == State::COMPLETE; }
};

