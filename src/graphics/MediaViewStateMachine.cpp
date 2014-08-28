#include "stdafx.h"
#include "uievents/oscDomain.h"
#include "MediaViewStateMachine.h"
#include "uievents/MediaViewEventListener.h"

const static float PI = 3.14159265f;

MediaViewStateMachine::MediaViewStateMachine() :
m_state(State::INACTIVE) {}

void MediaViewStateMachine::AutoFilter(OSCState appState, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll) {
  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    m_mediaView->CloseMenu();
    return;
  }
  
  switch( m_state )
  {
    case State::INACTIVE:
      if(appState == OSCState::MEDIA_MENU_FOCUSED) {
        m_mediaView->OpenMenu(handLocation);
        m_mediaViewEventListener(&MediaViewEventListener::OnInitializeVolume);
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_mediaView->CloseMenu();
        m_state = State::INACTIVE;
      }
      break;
    case State::SELECTION_MADE:
      m_mediaView->CloseMenu();
      m_state = State::FADE_OUT;
      break;
    case State::FADE_OUT:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_state = State::INACTIVE;
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
      break;
    case State::ACTIVE:
    {
      std::shared_ptr<Wedge> activeWedge = m_mediaView->GetActiveWedgeFromHandLocation(handLocation);
      float distance = m_mediaView->GetDistanceFromCenter(handLocation);
      m_mediaView->UpdateWedges(activeWedge, distance);
      m_mediaViewEventListener(&MediaViewEventListener::OnUserChangedVolume)(calculateVolumeDelta(dHandRoll.dTheta));
      break;
    }
    case State::SELECTION_MADE:
      //something
      break;
    case State::FINAL:
    default:
      break;
  }
  
}

float MediaViewStateMachine::calculateVolumeDelta(float deltaHandRoll) {
  return deltaHandRoll / (3 * PI / 2.0);
}

//After Selection
/*
 activeWedge->OnSelected();
 m_state = State::SELECTION_MADE;
*/
