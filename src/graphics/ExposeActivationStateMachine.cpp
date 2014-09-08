#include "stdafx.h"
#include "ExposeActivationStateMachine.h"
#include "RenderState.h"
#include "RenderFrame.h"

const double startAngle = 5 * M_PI/4.0;
const double endAngle = startAngle + 2 * (M_PI/4.0);
const int numItems = 1;
const Color bgColor(0.4f, 0.425f, 0.45f, 0.75f);
const Color fillColor(0.505f, 0.831f, 0.114f, 0.95f);
const Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Color handleOutlineColor(0.505f, 0.831f, 0.114f, 0.75f);

ExposeActivatorEventListener::ExposeActivatorEventListener() { }
ExposeActivatorEventListener::~ExposeActivatorEventListener() { }


ExposeActivationStateMachine::ExposeActivationStateMachine() :
m_radialMenu(new RadialMenu()),
m_state(State::INACTIVE)
{
  m_CurrentTime = 0.0;
  m_LastStateChangeTime = 0.0;
  m_FadeTime = 0.25;
  m_selectedItem = -1;
  
  //Radial Menu Initialization
  m_radialMenu->SetStartAngle(startAngle);
  m_radialMenu->SetEndAngle(endAngle);
  m_radialMenu->SetNumItems(numItems);
  m_radialMenu->SetRadius(120.0);
  m_radialMenu->SetThickness(70.0);
  for (int i=0; i<numItems; i++) {
    //TODO: Break these out into a config to avoid so many magic numbers
    std::shared_ptr<RadialMenuItem>& item = m_radialMenu->GetItem(i);
    item->SetRadius(120.0);
    item->SetThickness(80.0);
    item->SetActivatedRadius(160.0);
    item->Material().SetAmbientLightColor(bgColor);
    item->Material().SetAmbientLightingProportion(1.0f);
    item->Material().SetDiffuseLightColor(bgColor);
    item->SetHoverColor(fillColor);
    item->SetActivatedColor(handleOutlineColor);
  }
  
  //TODO: Setup SVGs
}


ExposeActivationStateMachine::~ExposeActivationStateMachine() { }

void ExposeActivationStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void ExposeActivationStateMachine::AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  m_CurrentTime += 1E-6 * frameTime.deltaTime;
  
  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }
  
  switch( m_state )
  {
    case State::INACTIVE:
      if(appState == OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_radialMenu->Translation() = Vector3(handData.locationData.x, handData.locationData.y, 0.0);
        m_state = State::ACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_state = State::INACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::SELECTION_MADE:
      m_state = State::FADE_OUT;
      m_LastStateChangeTime = m_CurrentTime;
      break;
    case State::FADE_OUT:
      if(appState != OSCState::EXPOSE_ACTIVATOR_FOCUSED) {
        m_state = State::INACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::FINAL:
    default:
      break;
  }
  
  // State Loops
  switch (m_state) {
    case State::INACTIVE:
      // Wedge transparency is updated in AnimationUpdate loops
      m_radialMenu->InteractWithoutCursor();
      m_selectedItem = -1;
      break;
    case State::ACTIVE:
    {
      // MENU UPDATE
      
      // The menu always thinks it's at (0,0) so we need to offset the cursor
      // coordinates by the location of the menu to give the proper space.
      const Vector2 menuOffset = m_radialMenu->Translation().head<2>();
      
      Vector3 leapPosition(handData.locationData.x - menuOffset.x(), handData.locationData.y - menuOffset.y(), 0);
      RadialMenu::UpdateResult updateResult = m_radialMenu->InteractWithCursor(leapPosition);
      m_selectedItem = updateResult.updateIdx;
      if(updateResult.curActivation >= 0.95) { // the component doesn't always return a 1.0 activation. Not 100% sure why.
        //Selection Made Transition
        resolveSelection(updateResult.updateIdx);
        m_state = State::SELECTION_MADE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    }
    case State::SELECTION_MADE:
    case State::FINAL:
    default:
      break;
  }
  for (int i=0; i<numItems; i++) {
    m_radialMenu->GetItem(i)->SetOverrideOpacity(i == m_selectedItem);
  }
  m_radialMenu->UpdateItemActivation(static_cast<float>(1E-6 * frameTime.deltaTime));
}

void ExposeActivationStateMachine::resolveSelection(int selectedID) {
  switch(selectedID) {
    case 0:
      m_exposeActivatorEventListener(&ExposeActivatorEventListener::OnActivateExpose)();
      break;
    default:
      break;
  }
}

void ExposeActivationStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {
  float opacity = 0.0f;
  if (m_state == State::ACTIVE) {
    // fade in
    opacity = SmootherStep(std::min(1.0f, static_cast<float>((m_CurrentTime - m_LastStateChangeTime)/m_FadeTime)));
  } else if (m_state == State::FADE_OUT || m_state == State::SELECTION_MADE || m_state == State::INACTIVE) {
    // fade out
    opacity = SmootherStep(1.0f-std::min(1.0f, static_cast<float>((m_CurrentTime - m_LastStateChangeTime)/m_FadeTime)));
    if (m_selectedItem >= 0) {
      const float itemOpacity = SmootherStep(1.0f-std::min(1.0f, static_cast<float>((m_CurrentTime - 2*m_FadeTime - m_LastStateChangeTime)/m_FadeTime)));
      m_radialMenu->GetItem(m_selectedItem)->SetOpacity(itemOpacity);
    } else {
      for (int i=0; i<numItems; i++) {
        m_radialMenu->GetItem(i)->SetOpacity(1.0f);
      }
    }
  }
  m_radialMenu->SetOpacity(opacity);
}

void ExposeActivationStateMachine::Render(const RenderFrame &renderFrame) const  {
  if (m_state == State::ACTIVE || m_state == State::SELECTION_MADE || m_state == State::FADE_OUT) {
    PrimitiveBase::DrawSceneGraph(*m_radialMenu, renderFrame.renderState);
  }
}

