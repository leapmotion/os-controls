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
const size_t numItems = 3;
const Color bgColor(0.4f, 0.425f, 0.45f, 0.75f);
const Color fillColor(0.4f, 0.8f, 0.4f, 0.7f);
const Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Color handleOutlineColor(0.6f, 1.0f, 0.6f, 1.0f);

MediaViewStateMachine::MediaViewStateMachine() :
m_state(State::INACTIVE) {
  
  //Radial Menu Initialization
  m_radialMenu.SetStartAngle(startAngle);
  m_radialMenu.SetEndAngle(endAngle);
  m_radialMenu.SetNumItems(numItems);
  m_radialMenu.SetRadius(120.0);
  m_radialMenu.SetThickness(70.0);
  for (int i=0; i<numItems; i++) {
    //TODO: Break these out into a config to avoid so many magic numbers
    std::shared_ptr<RadialMenuItem>& item = m_radialMenu.GetItem(i);
    item->SetRadius(120.0);
    item->SetThickness(80.0);
    item->SetActivatedRadius(200.0);
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
  std::shared_ptr<SVGPrimitive> volumeIcon(new SVGPrimitive());
  
  nextIcon->Set(nextIconFile->Contents());
  playPauseIcon->Set(playPauseIconFile->Contents());
  prevIcon->Set(prevIconFile->Contents());
  volumeIcon->Set(volumeIconFile->Contents());
  
  m_radialMenu.GetItem(0)->SetIcon(prevIcon);
  m_radialMenu.GetItem(1)->SetIcon(playPauseIcon);
  m_radialMenu.GetItem(2)->SetIcon(nextIcon);
  
  //Volume Slider Initilization
  m_volumeSlider.SetRadius(60.0);
  m_volumeSlider.SetThickness(10.0);
  m_volumeSlider.SetStartAngle(startAngle);
  m_volumeSlider.SetEndAngle(endAngle);
  m_volumeSlider.SetFillColor(fillColor);
  m_volumeSlider.SetHandleColor(handleColor);
  m_volumeSlider.SetHandleOutlineColor(handleOutlineColor);
  m_volumeSlider.Material().SetDiffuseLightColor(bgColor);
}

void MediaViewStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void MediaViewStateMachine::AutoFilter(OSCState appState, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll, const FrameTime& frameTime) {
  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }
  
  switch( m_state )
  {
    case State::INACTIVE:
      if(appState == OSCState::MEDIA_MENU_FOCUSED) {
        m_volumeSlider.Translation() = Vector3(handLocation.x, handLocation.y, 0.0);
        m_radialMenu.Translation() = Vector3(handLocation.x, handLocation.y, 0.0);
        m_mediaViewEventListener(&MediaViewEventListener::OnInitializeVolume);
        m_startRoll = dHandRoll.absoluteRoll;
        m_hasRoll = true;
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_state = State::INACTIVE;
      }
      break;
    case State::SELECTION_MADE:
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
      // MENU UPDATE
      
      // The menu always thinks it's at (0,0) so we need to offset the cursor
      // coordinates by the location of the menu to give the proper space.
      const Vector2 menuOffset = m_radialMenu.Translation().head<2>();
      
      Vector3 leapPosition(handLocation.x - menuOffset.x(), handLocation.y - menuOffset.y(), 0);
      RadialMenu::UpdateResult updateResult = m_radialMenu.UpdateItemsFromCursor(leapPosition, static_cast<float>(1E-6 * frameTime.deltaTime));
      if(updateResult.curActivation >= 0.95) { // the component doesn't always return a 1.0 activation. Not 100% sure why.
        //Selection Made Transition
        resolveSelection(updateResult.updateIdx);
        m_state = State::SELECTION_MADE;
      }
      
      // VOLUME UPDATE
      //m_mediaViewEventListener(&MediaViewEventListener::OnUserChangedVolume)(calculateVolumeDelta(dHandRoll.dTheta));
      const float DEADZONE = 0.4f;
      const float MAX = 1.0f;
      const float MAX_VELOCTY = 0.7f;
      
      if( !m_hasRoll ) {
        m_startRoll = dHandRoll.absoluteRoll;
        m_hasRoll = true;
        return;
      }
      
      float offset = dHandRoll.absoluteRoll - m_startRoll;
      int sign = offset < 0 ? -1 : 1;
      float norm = (fabs(offset) - DEADZONE) / (MAX - DEADZONE);
      norm = std::min(1.0f, std::max(0.0f, norm));
      float velocity = norm * MAX_VELOCTY * sign * (frameTime.deltaTime / 100000.0f);
      std::cout << "time: " << frameTime.deltaTime << std::endl;
      std::cout << "norm: " << norm << std::endl;
      std::cout << " vel: " << velocity << std::endl;
      m_mediaViewEventListener(&MediaViewEventListener::OnUserChangedVolume)(calculateVolumeDelta(velocity));
      break;
    }
    case State::SELECTION_MADE:
    case State::FINAL:
    default:
      break;
  }
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
  m_volumeSlider.SetValue(volume);
}

void MediaViewStateMachine::Render(const RenderFrame &renderFrame) const  {
  if(m_state == State::ACTIVE) {
    PrimitiveBase::DrawSceneGraph(m_radialMenu, renderFrame.renderState);
    PrimitiveBase::DrawSceneGraph(m_volumeSlider, renderFrame.renderState);
  }
}

//TODO: Filter this data in the recognizer to smooth things out.
float MediaViewStateMachine::calculateVolumeDelta(float deltaHandRoll) {
  return (float) (deltaHandRoll / (3 * PI / 2.0));
}