#include "stdafx.h"
#include "ExposeActivationStateMachine.h"
#include "graphics/RenderFrame.h"

#include "RenderState.h"

ExposeActivationStateMachine::ExposeActivationStateMachine() :
  m_state(State::INACTIVE),
  m_goalStrip(new RectanglePrim()),
  m_pusherBar(new RectanglePrim()),
  m_armed(false)
{
  m_goalBottomY.SetInitialValue(0.0f);
  m_pusherBottomY.SetInitialValue(0.0f);
  
  m_goalBottomY.SetSmoothStrength(0.7f);
  m_pusherBottomY.SetSmoothStrength(0.7f);
}


ExposeActivationStateMachine::~ExposeActivationStateMachine() { }

void ExposeActivationStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void ExposeActivationStateMachine::AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }
  
  switch( m_state )
  {
    case State::INACTIVE:
      //Transition to ACTIVE
      if(appState == OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_goalBottomY.SetGoal(GOAL_BOTTOM_Y);
        m_pusherBottomY.SetGoal(PUSHER_BOTTOM_Y);
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      //Transition to INACTIVE
      if(appState != OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        transitionToInactive();
      }
      break;
    case State::COMPLETE:
      //TRANSITION TO INACTIVE
      if(appState != OSCState::EXPOSE_ACTIVATOR_FOCUSED && appState != OSCState::EXPOSE_FOCUSED) {
        transitionToInactive();
      }
      break;
    case State::FINAL:
    default:
      break;
  }
  
  // State Loops
  switch (m_state) {
    case State::INACTIVE:
      break;
    case State::ACTIVE:
    {
      float yDiff = PUSHER_BOTTOM_Y - handData.locationData.y;
      float diffPercent = yDiff / PUSHER_BOTTOM_Y;
      diffPercent = std::min(1.0f, std::max(0.0f, diffPercent));
      Color blended = blendColor(UNSELECTED_COLOR, SELECTED_COLOR, diffPercent);
      m_pusherBottomY.SetInitialValue( std::min(handData.locationData.y, PUSHER_BOTTOM_Y) );
      m_pusherBottomY.SetGoal( std::min(handData.locationData.y, PUSHER_BOTTOM_Y) );
      m_pusherBar->Material().SetAmbientLightColor(blended);
      m_pusherBar->Material().SetDiffuseLightColor(blended);
      m_pusherBar->Material().SetAmbientLightColor(blended);
      
      if( diffPercent >= 1) {
        resolveSelection();
        transitionToInactive();
      }
    }
    case State::FINAL:
    default:
      break;
  }
}

Color ExposeActivationStateMachine::blendColor(Color c1, Color c2, float amnt) {
  Color retColor = Color();
  
  amnt = std::min(1.0f, std::max(0.0f, amnt));
  
  retColor.R() = ( amnt * c1.R()  ) + ( (1-amnt) * c2.R() );
  retColor.G() = ( amnt * c1.G()  ) + ( (1-amnt) * c2.G() );
  retColor.B() = ( amnt * c1.B()  ) + ( (1-amnt) * c2.B() );
  retColor.A() = ( amnt * c1.A()  ) + ( (1-amnt) * c2.A() );
  
  return retColor;
}

void ExposeActivationStateMachine::transitionToInactive() {
  m_goalBottomY.SetGoal(0.0f);
  m_pusherBottomY.SetGoal(0.0f);
  m_state = State::INACTIVE;
}

void ExposeActivationStateMachine::resolveSelection() {
  m_stateChangeEvent(&OSCStateChangeEvent::RequestTransition)(OSCState::EXPOSE_FOCUSED);
}

void ExposeActivationStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {
  m_goalBottomY.Update(renderFrame.deltaT.count());
  m_pusherBottomY.Update(renderFrame.deltaT.count());
  
  float barWidth = m_renderWindow->getSize().x;
  float goalStripY = m_goalBottomY - (GOAL_BOTTOM_Y/2.0f);
  float pusherStripY = m_pusherBottomY - (PUSHER_BOTTOM_Y/2.0f);
  
  m_pusherBar->SetSize(Vector2(barWidth, PUSHER_BOTTOM_Y));
  m_goalStrip->SetSize(Vector2(barWidth, GOAL_BOTTOM_Y));
  
  m_pusherBar->Translation() = Vector3(0.0f, pusherStripY, 0.0f);
  m_goalStrip->Translation() = Vector3(0.0f, goalStripY, 0.0f);
}

void ExposeActivationStateMachine::Render(const RenderFrame &renderFrame) const  {
  if (m_state == State::ACTIVE || m_state == State::COMPLETE) {
    PrimitiveBase::DrawSceneGraph(*m_pusherBar, renderFrame.renderState);
    PrimitiveBase::DrawSceneGraph(*m_goalStrip, renderFrame.renderState);
  }
}

