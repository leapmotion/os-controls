#pragma once
#include "graphics/Renderable.h"
#include "graphics/RenderEngine.h"
#include "interaction/HandDataCombiner.h"
#include "uievents/OSCDomain.h"

#include <RadialMenu.h>
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
  void resolveSelection(int selectedID);
  
  enum class State {
    
    /*                        |----------V
     *    --> Inactive --> Active --> SelectionMade
     *           ^-----------|-----------|
     */
    
    //Media View is created but not focused.
    INACTIVE,
    
    //Taking user input, fading in, etc
    ACTIVE,
    
    //Done taking input, has sent its event up the chain. Mostly for finished animations.
    SELECTION_MADE,
    
    //Wait for animation to fade out
    FADE_OUT,
    
    //Tear everything down.
    FINAL
  };
  
  std::shared_ptr<RadialMenu>m_radialMenu;
  State m_state;
  
  Autowired<RenderEngine> m_rootNode;
  AutoFired<OSCStateChangeEvent> m_stateChangeEvent;
  
  int m_selectedItem;
  double m_FadeTime;
  double m_CurrentTime;
  double m_LastStateChangeTime;
};