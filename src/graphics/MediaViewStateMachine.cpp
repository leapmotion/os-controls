#include "stdafx.h"
#include "uievents/oscDomain.h"
#include "MediaViewStateMachine.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/osControlConfigs.h"
#include "RenderEngine.h"
#include "RenderState.h"
#include "RenderFrame.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTexture2.h"
#include "TextFile.h"
#include "Resource.h"
#include <memory>

const static float PI = 3.14159265f;

const double startAngle = 3 * M_PI/4.0;
const double endAngle = startAngle + 6 * (M_PI/4.0);
const int numItems = 3;
const Color bgColor(0.4f, 0.425f, 0.45f, 0.75f);
const Color fillColor(0.505f, 0.831f, 0.114f, 0.95f);
const Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Color handleOutlineColor(0.505f, 0.831f, 0.114f, 0.75f);

MediaViewStateMachine::MediaViewStateMachine() :
m_radialMenu(new RadialMenu()),
m_lastHandPose(HandPose::ZeroFingers),
m_state(State::ARMED) {
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
  
  Resource<TextFile> nextIconFile("next-track-icon-extended-01.svg");
  Resource<TextFile> playPauseIconFile("play_pause-icon-extended-01.svg");
  Resource<TextFile> prevIconFile("prev-track-icon-extended-01.svg");
  Resource<TextFile> volumeIconFile("volume-icon-01.svg");
  
  std::shared_ptr<SVGPrimitive> nextIcon(new SVGPrimitive());
  std::shared_ptr<SVGPrimitive> playPauseIcon(new SVGPrimitive());
  std::shared_ptr<SVGPrimitive> prevIcon(new SVGPrimitive());
  
  nextIcon->Set(nextIconFile->Contents());
  playPauseIcon->Set(playPauseIconFile->Contents());
  prevIcon->Set(prevIconFile->Contents());
  
  m_radialMenu->GetItem(0)->SetIcon(prevIcon);
  m_radialMenu->GetItem(1)->SetIcon(playPauseIcon);
  m_radialMenu->GetItem(2)->SetIcon(nextIcon);
}

void MediaViewStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void MediaViewStateMachine::AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  m_CurrentTime += 1E-6 * frameTime.deltaTime;

  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }
  
  m_lastHandPose = handData.handPose;
  
  switch( m_state )
  {
    case State::ARMED:
      if(appState == OSCState::MEDIA_MENU_FOCUSED) {
        m_radialMenu->Translation() = Vector3(handData.locationData.x, handData.locationData.y, 0.0);
        m_mediaViewEventListener(&MediaViewEventListener::OnInitializeVolume)();
        m_startRoll = handData.rollData.absoluteRoll;
        m_hasRoll = true;
        m_state = State::ACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_state = State::ARMED;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::COMPLETE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_state = State::ARMED;
        m_LastStateChangeTime = m_CurrentTime;

      }
      break;
    case State::FINAL:
    default:
      break;
  }

  // State Loops
  switch (m_state) {
    case State::ARMED:
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
        m_state = State::COMPLETE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    }
    case State::FINAL:
    default:
      break;
  }
  for (int i=0; i<numItems; i++) {
    // The apply type indicates how the composition of properties along the line of
    // ancestry in the scene graph works.  ApplyType::OPERATE is ordinary composition
    // (e.g. multiplication of coordinate transformations).  ApplyType::REPLACE
    // is an override of the existing value, and is how the alpha mask is overridden
    // for the selected item.
    ApplyType apply_type = i == m_selectedItem ? ApplyType::REPLACE : ApplyType::OPERATE;
    m_radialMenu->GetItem(i)->LocalProperties().AlphaMaskProperty().SetApplyType(apply_type);
  }
  m_radialMenu->UpdateItemActivation(static_cast<float>(1E-6 * frameTime.deltaTime));
}

void MediaViewStateMachine::resolveSelection(int selectedID) {
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

void MediaViewStateMachine::SetViewVolume(float volume) {
  volume = 1 - volume;
  //m_volumeSlider->SetValue(volume);
}

void MediaViewStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {
  float alphaMask = 0.0f;
  if (m_state == State::ACTIVE) {
    // fade in
    alphaMask = SmootherStep(std::min(1.0f, static_cast<float>((m_CurrentTime - m_LastStateChangeTime)/m_FadeTime)));
  } else if (m_state == State::COMPLETE || m_state == State::ARMED) {
    // fade out
    alphaMask = SmootherStep(1.0f-std::min(1.0f, static_cast<float>((m_CurrentTime - m_LastStateChangeTime)/m_FadeTime)));
    if (m_selectedItem >= 0) {
      const float itemAlphaMask = SmootherStep(1.0f-std::min(1.0f, static_cast<float>((m_CurrentTime - 2*m_FadeTime - m_LastStateChangeTime)/m_FadeTime)));
      m_radialMenu->GetItem(m_selectedItem)->LocalProperties().AlphaMask() = itemAlphaMask;
    } else {
      for (int i=0; i<numItems; i++) {
        m_radialMenu->GetItem(i)->LocalProperties().AlphaMask() = 1.0f;
      }
    }
  }
  m_radialMenu->LocalProperties().AlphaMask() = alphaMask;
}

void MediaViewStateMachine::Render(const RenderFrame &renderFrame) const  {
  if (m_state == State::ACTIVE || m_state == State::COMPLETE) {
    if ( m_lastHandPose == HandPose::OneFinger )
    {
      PrimitiveBase::DrawSceneGraph(*m_radialMenu, renderFrame.renderState);
    }
    else {
    }
  }
}

//TODO: Filter this data in the recognizer to smooth things out.
float MediaViewStateMachine::calculateVolumeDelta(float deltaHandRoll) {
  return deltaHandRoll / static_cast<float>(3 * PI / 2.0);
}
