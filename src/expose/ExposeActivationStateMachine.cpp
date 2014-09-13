#include "stdafx.h"
#include "ExposeActivationStateMachine.h"
#include "graphics/RenderFrame.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTexture2.h"
#include "TextFile.h"
#include "Resource.h"
#include <memory>

#include "RenderState.h"

ExposeActivationStateMachine::ExposeActivationStateMachine() :
  m_state(State::INACTIVE),
  m_goalStrip(new RectanglePrim()),
  m_pusherBar(new RectanglePrim()),
  m_exposeIcon(new SVGPrimitive()),
  m_armed(false)
{
  m_goalBottomY.SetInitialValue(0.0f);
  m_pusherBottomY.SetInitialValue(0.0f);
  
  m_goalBottomY.SetSmoothStrength(0.7f);
  m_pusherBottomY.SetSmoothStrength(0.7f);
  
  m_goalStrip->Material().SetDiffuseLightColor(GOAL_COLOR);
  m_goalStrip->Material().SetAmbientLightColor(GOAL_COLOR);
  m_goalStrip->Material().SetAmbientLightingProportion(1.0f);
  
  // Setup SVG
  Resource<TextFile> exposeIconFile("expose-icon-01.svg");
  m_exposeIcon->Set(exposeIconFile->Contents());
  m_exposeIconOffset = m_exposeIcon->Origin() - (m_exposeIcon->Size() / 2.0f);
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
      m_goalBottomY.SetInitialValue(0.0f);
      m_pusherBottomY.SetInitialValue(0.0f);
      m_goalBottomY.SetGoal(0.0f);
      m_pusherBottomY.SetGoal(0.0f);
      break;
    case State::ACTIVE:
    {
      float yDiff = PUSHER_BOTTOM_Y - handData.locationData.y;
      float diffPercent = yDiff / PUSHER_BOTTOM_Y;
      diffPercent = std::min(1.0f, std::max(0.0f, diffPercent));
      Color blended = blendColor(UNSELECTED_COLOR, SELECTED_COLOR, diffPercent);
      if ( diffPercent > 0 ) {
        if ( m_armed ) {
          m_pusherBottomY.SetInitialValue( std::min(handData.locationData.y, PUSHER_BOTTOM_Y) );
          m_pusherBottomY.SetGoal( std::min(handData.locationData.y, PUSHER_BOTTOM_Y) );
        }
      }
      else {
        m_armed = true;
        m_pusherBottomY.SetGoal( PUSHER_BOTTOM_Y );
      }
      m_pusherBar->Material().SetAmbientLightColor(blended);
      m_pusherBar->Material().SetDiffuseLightColor(blended);
      m_pusherBar->Material().SetAmbientLightingProportion(1.0f);
      
      if( diffPercent >= 1 && m_armed) {
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
  amnt = std::min(1.0f, std::max(0.0f, amnt));
  const Vector4f blend = (amnt * c2.Data()) + ((1.0f-amnt) * c1.Data());
  return Color(blend);
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
  float screenMiddle = m_renderWindow->getSize().x/2.0f;
  
  m_pusherBar->SetSize(Vector2(barWidth, PUSHER_BOTTOM_Y));
  m_goalStrip->SetSize(Vector2(barWidth, GOAL_BOTTOM_Y));
  
  m_pusherBar->Translation() = Vector3(screenMiddle, pusherStripY, 0.0f);
  m_goalStrip->Translation() = Vector3(screenMiddle, goalStripY, 0.0f);
  m_exposeIcon->Translation() = Vector3(m_exposeIconOffset.x() + screenMiddle, m_exposeIconOffset.y() + pusherStripY + ICON_Y_OFFSET, 0.0f);
}

void ExposeActivationStateMachine::Render(const RenderFrame &renderFrame) const  {
  PrimitiveBase::DrawSceneGraph(*m_pusherBar, renderFrame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_exposeIcon, renderFrame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_goalStrip, renderFrame.renderState);
}