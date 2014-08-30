#include "stdafx.h"
#include "uievents/oscDomain.h"
#include "MediaViewStateMachine.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/osControlConfigs.h"
#include "RenderState.h"
#include "RenderFrame.h"

const static float PI = 3.14159265f;

const double startAngle = 3 * M_PI/4.0;
const double endAngle = startAngle + 6 * (M_PI/4.0);
const double numItems = 3;
const Color bgColor(0.4f, 0.425f, 0.45f, 0.75f);
const Color fillColor(0.4f, 0.8f, 0.4f, 0.7f);
const Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Color handleOutlineColor(0.6f, 1.0f, 0.6f, 1.0f);

//const double volumeRadius = 8.625;
//const double volumeThickness = 0.75;

MediaViewStateMachine::MediaViewStateMachine() :
m_state(State::INACTIVE) {
  m_radialMenu.SetStartAngle(startAngle);
  m_radialMenu.SetEndAngle(endAngle);
  m_radialMenu.SetNumItems(numItems);
  m_radialMenu.SetRadius(120.0);
  m_radialMenu.SetThickness(70.0);
  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_radialMenu.GetItem(i);
    item->SetRadius(120.0);
    item->SetThickness(80.0);
    item->SetActivatedRadius(200.0);
    item->Material().SetDiffuseLightColor(bgColor);
    item->SetHoverColor(fillColor);
    item->SetActivatedColor(handleOutlineColor);
  }
  
  //TODO: Setup the SVGs for the menu
}

void MediaViewStateMachine::AutoInit() {
  auto self = shared_from_this();
  m_rootNode->AddChild(self);
}

void MediaViewStateMachine::AutoFilter(OSCState appState, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll, const FrameTime& frameTime) {
  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    //m_mediaView->CloseMenu();
    return;
  }
  
  switch( m_state )
  {
    case State::INACTIVE:
      if(appState == OSCState::MEDIA_MENU_FOCUSED) {
        //m_mediaView->OpenMenu(handLocation);
        m_radialMenu.Translation() = Vector3(handLocation.x, handLocation.y, 0.0);
        m_mediaViewEventListener(&MediaViewEventListener::OnInitializeVolume);
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        //m_mediaView->CloseMenu();
        m_state = State::INACTIVE;
      }
      break;
    case State::SELECTION_MADE:
      //m_mediaView->CloseMenu();
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
      // The menu always thinks it's at (0,0) so we need to offset the cursor
      // coordinates by the location of the menu to give the proper space.
      const Vector2 menuOffset = m_radialMenu.Translation().head<2>();
      
      Vector3 leapPosition(handLocation.x - menuOffset.x(), handLocation.y - menuOffset.y(), 0);
      RadialMenu::UpdateResult updateResult = m_radialMenu.UpdateItemsFromCursor(leapPosition, frameTime.deltaTime);
      m_mediaViewEventListener(&MediaViewEventListener::OnUserChangedVolume)(calculateVolumeDelta(dHandRoll.dTheta));
      std::cout << updateResult.curActivation << std::endl;
      if(updateResult.curActivation >= 0.95) { // the component doesn't always return a 1.0 activation. Not 100% sure why.
        //Selection Made Transition
        resolveSelection(updateResult.updateIdx);
        m_state = State::SELECTION_MADE;
      }
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

void MediaViewStateMachine::resolveSelection(int selectedID) {
  std::cout << "SelectID: " << selectedID << std::endl;
  switch(selectedID) {
    case 0:
      m_mediaViewEventListener(&MediaViewEventListener::OnUserPrevTrack)();
      break;
    case 1:
      m_mediaViewEventListener(&MediaViewEventListener::OnUserPlayPause)();
      break;
    case 2:
      m_mediaViewEventListener(&MediaViewEventListener::OnUserNextTrack)();
      break;
  }
}

void MediaViewStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {

}

void MediaViewStateMachine::Render(const RenderFrame &renderFrame) const  {
  if(m_state == State::ACTIVE) {
    PrimitiveBase::DrawSceneGraph(m_radialMenu, renderFrame.renderState);
  }
}

float MediaViewStateMachine::calculateVolumeDelta(float deltaHandRoll) {
  return deltaHandRoll / (3 * PI / 2.0);
}