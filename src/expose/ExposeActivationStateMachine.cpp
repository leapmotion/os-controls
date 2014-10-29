#include "stdafx.h"
#include "ExposeActivationStateMachine.h"
#include "osinterface/RenderWindow.h"
#include "graphics/RenderFrame.h"

#include "GLShaderLoader.h"
#include "Leap/GL/GLShader.h"
#include "Leap/GL/GLTexture2.h"
#include "Resource.h"
#include "TextFile.h"
#include <memory>

#include "RenderState.h"

ExposeActivationStateMachine::ExposeActivationStateMachine() :
  m_state(State::INACTIVE),
  m_goalStrip(new RectanglePrim()),
  m_pusherBar(new RectanglePrim()),
  m_exposeIcon(new SVGPrimitive()),
  m_armed(false),
  m_goalBottomY(0.0f,0.7f),
  m_pusherBottomY(0.0f, 0.7f)
{
  m_goalStrip->Material().SetDiffuseLightColor(GOAL_COLOR);
  m_goalStrip->Material().SetAmbientLightColor(GOAL_COLOR);
  m_goalStrip->Material().SetAmbientLightingProportion(1.0f);

  // Setup SVG
  Resource<TextFile> exposeIconFile("expose-icon-01.svg");
  m_exposeIcon->Set(exposeIconFile->Contents());
  m_exposeIconOffset = m_exposeIcon->Origin() - (m_exposeIcon->Size() / 2.0f);

  AutoCurrentContext()->AddTeardownListener([this](){
    RemoveFromParent();
  });
}


ExposeActivationStateMachine::~ExposeActivationStateMachine() { }

void ExposeActivationStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void ExposeActivationStateMachine::AutoFilter(ShortcutsState appState, const HandData& handData, const FrameTime& frameTime) {
  // State Transitions
  if (appState == ShortcutsState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }

  switch( m_state )
  {
    case State::INACTIVE:
      //Transition to ACTIVE
      if(appState == ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_goalBottomY.SetGoal(GOAL_BOTTOM_Y);
        m_pusherBottomY.SetGoal(PUSHER_BOTTOM_Y);
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      //Transition to INACTIVE
      if(appState != ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED) {
        transitionToInactive();
      }
      break;
    case State::COMPLETE:
      //TRANSITION TO INACTIVE
      if(appState != ShortcutsState::EXPOSE_ACTIVATOR_FOCUSED && appState != ShortcutsState::EXPOSE_FOCUSED) {
        transitionToInactive();
      }
      break;
    case State::FINAL:
      // NOTE: This is probably not called.
      // Instead, the call-back in AddTearDownListener will be called.
      RemoveFromParent();
      break;
    default:
      break;
  }

  // State Loops
  switch (m_state) {
    case State::INACTIVE:
      m_goalBottomY.SetImmediate(0.0f);
      m_pusherBottomY.SetImmediate(0.0f);
      break;
    case State::ACTIVE:
    {
      float yDiff = PUSHER_BOTTOM_Y - handData.locationData.y;
      float diffPercent = yDiff / PUSHER_BOTTOM_Y;
      diffPercent = std::min(1.0f, std::max(0.0f, diffPercent));
      Color blended = blendColor(UNSELECTED_COLOR, SELECTED_COLOR, diffPercent);
      if ( diffPercent > 0 ) {
        if ( m_armed ) {
          m_pusherBottomY.SetImmediate(std::min(handData.locationData.y, PUSHER_BOTTOM_Y));
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
  const EigenTypes::Vector4f blend = (amnt * c2.Data()) + ((1.0f-amnt) * c1.Data());
  return Color(blend);
}

void ExposeActivationStateMachine::transitionToInactive() {
  m_goalBottomY.SetGoal(0.0f);
  m_pusherBottomY.SetGoal(0.0f);
  m_state = State::INACTIVE;
}

void ExposeActivationStateMachine::resolveSelection() {
  m_stateChangeEvent(&ShortcutsStateChangeEvent::RequestTransition)(ShortcutsState::EXPOSE_FOCUSED);
}

void ExposeActivationStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {
  m_goalBottomY.Update(static_cast<float>(renderFrame.deltaT.count()));
  m_pusherBottomY.Update(static_cast<float>(renderFrame.deltaT.count()));

  float barWidth = m_renderWindow->GetSize().width;
  float goalStripY = m_goalBottomY - (GOAL_BOTTOM_Y/2.0f);
  float pusherStripY = m_pusherBottomY - (PUSHER_BOTTOM_Y/2.0f);
  float screenMiddle = barWidth/2.0f;

  m_pusherBar->SetSize(EigenTypes::Vector2(barWidth, PUSHER_BOTTOM_Y));
  m_goalStrip->SetSize(EigenTypes::Vector2(barWidth, GOAL_BOTTOM_Y));

  m_pusherBar->Translation() = EigenTypes::Vector3(screenMiddle, pusherStripY, 0.0f);
  m_goalStrip->Translation() = EigenTypes::Vector3(screenMiddle, goalStripY, 0.0f);
  m_exposeIcon->Translation() = EigenTypes::Vector3(m_exposeIconOffset.x() + screenMiddle, m_exposeIconOffset.y() + pusherStripY + ICON_Y_OFFSET, 0.0f);
}

void ExposeActivationStateMachine::Render(const RenderFrame &renderFrame) const  {
  PrimitiveBase::DrawSceneGraph(*m_pusherBar, renderFrame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_exposeIcon, renderFrame.renderState);
  PrimitiveBase::DrawSceneGraph(*m_goalStrip, renderFrame.renderState);
}
