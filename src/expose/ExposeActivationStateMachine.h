#pragma once
#include "graphics/Renderable.h"
#include "graphics/RenderEngine.h"
#include "interaction/HandDataCombiner.h"
#include "uievents/OSCDomain.h"

#include <autowiring/Autowiring.h>

class ExposeActivationStateMachine  :
public std::enable_shared_from_this<ExposeActivationStateMachine>,
public Renderable
{
public:
  ExposeActivationStateMachine();
  ~ExposeActivationStateMachine();
  
  void AutoInit();
  
  void AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime);
  
  void AnimationUpdate(const RenderFrame& renderFrame) override;
  void Render(const RenderFrame& renderFrame) const override;
private:
  void resolveSelection();
  
  enum class State {
    
    //Media View is created but not focused.
    INACTIVE,
    
    //Taking user input, fading in, etc
    ACTIVE,
    
    //Wait for animation to fade out
    COMPLETE,
    
    //Tear everything down.
    FINAL
  };
  
  State m_state;
  
  std::shared_ptr<RectanglePrim> m_goalStrip;
  std::shared_ptr<RectanglePrim> m_pusherBar;
  
  Autowired<RenderEngine> m_rootNode;
  AutoFired<OSCStateChangeEvent> m_stateChangeEvent;
};