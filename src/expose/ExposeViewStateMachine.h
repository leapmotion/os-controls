#pragma once
#include "expose/ExposeView.h"
#include "expose/ExposeViewWindow.h"
#include "interaction/HandLocationRecognizer.h"
#include "uievents/ShortcutsDomain.h"

class ExposeViewAccessManager;

class ExposeViewStateMachine :
  public ExposeViewEvents,
  public ContextMember
{
public:
  ExposeViewStateMachine();
  ~ExposeViewStateMachine();

  void AutoFilter(ShortcutsState appState, const HandData& handData);
  void onWindowSelected(ExposeViewWindow& osWindow) override;
  void Shutdown();
private:
  
  enum class State {
    INACTIVE,
    AWAITING_LOCK,
    ACTIVE
  };
  
  void doStateTransitions(ShortcutsState appState);
  void doStateLoops(const HandData& handData);
  void applyUserInput(const HandLocation& handLocation);
  
  State m_state;
  
  std::shared_ptr<ExposeView> m_exposeView;
  
  Autowired<ExposeViewAccessManager> m_exposeViewAccessManager;
  AutoFired<ShortcutsStateChangeEvent> m_stateChangeEvent;
};

