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
const Color fillColor(0.4f, 0.8f, 0.4f, 0.7f);
const Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Color handleOutlineColor(0.6f, 1.0f, 0.6f, 1.0f);

MediaViewStateMachine::MediaViewStateMachine() :
m_lastHandPose(HandPose::ZeroFingers),
m_state(State::INACTIVE) {
  m_CurrentTime = 0.0;
  m_LastStateChangeTime = 0.0;
  m_FadeTime = 0.25;
  m_selectedItem = -1;

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
  m_volumeSlider.Material().SetAmbientLightColor(bgColor);
  m_volumeSlider.Material().SetAmbientLightingProportion(1.0f);
}

void MediaViewStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void MediaViewStateMachine::AutoFilter(OSCState appState, const DeltaRollAmount& dra, const HandLocation& handLocation, const HandPose& handPose, const FrameTime& frameTime) {
  m_CurrentTime += 1E-6 * frameTime.deltaTime;

  // State Transitions
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    return;
  }
  
  //Hand Pose Transitions
  switch (m_lastHandPose) {
    case HandPose::OneFinger:
      if( handPose == HandPose::Clawed)
      {
        m_startRoll = dra.absoluteRoll;
      }
      break;
    case HandPose::Clawed:
      if ( handPose == HandPose::OneFinger)
      {
        //Update volume visual to 'unity' and update starting rotation
        m_volumeKnob->LinearTransformation() = Eigen::AngleAxis<double>(0.0, Vector3::UnitZ()).toRotationMatrix();
      }
    default:
      break;
  }
  
  m_lastHandPose = handPose;
  
  switch( m_state )
  {
    case State::INACTIVE:
      if(appState == OSCState::MEDIA_MENU_FOCUSED) {
        m_volumeSlider.Translation() = Vector3(handLocation.x, handLocation.y, 0.0);
        m_radialMenu.Translation() = Vector3(handLocation.x, handLocation.y, 0.0);
        m_volumeKnob->Translation() = Vector3(handLocation.x, handLocation.y, 0.0);
        m_mediaViewEventListener(&MediaViewEventListener::OnInitializeVolume)();
        m_startRoll = dra.absoluteRoll;
        m_hasRoll = true;
        m_volumeKnob->SetAlphaMaskGoal(0.5f);
        m_state = State::ACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_volumeKnob->SetAlphaMaskGoal(0.0f);
        m_state = State::INACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
      }
      break;
    case State::SELECTION_MADE:
      m_volumeKnob->SetAlphaMaskGoal(0.0f);
      m_state = State::FADE_OUT;
      m_LastStateChangeTime = m_CurrentTime;
      break;
    case State::FADE_OUT:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_volumeKnob->SetAlphaMaskGoal(0.0f);
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
      m_radialMenu.InteractWithoutCursor();
      m_selectedItem = -1;
      break;
    case State::ACTIVE:
    {

      if( handPose != HandPose::Clawed ) {
        // MENU UPDATE
        
        // The menu always thinks it's at (0,0) so we need to offset the cursor
        // coordinates by the location of the menu to give the proper space.
        const Vector2 menuOffset = m_radialMenu.Translation().head<2>();
        
        Vector3 leapPosition(handLocation.x - menuOffset.x(), handLocation.y - menuOffset.y(), 0);
        RadialMenu::UpdateResult updateResult = m_radialMenu.InteractWithCursor(leapPosition);
        m_selectedItem = updateResult.updateIdx;
        if(updateResult.curActivation >= 0.95) { // the component doesn't always return a 1.0 activation. Not 100% sure why.
          //Selection Made Transition
          resolveSelection(updateResult.updateIdx);
          m_state = State::SELECTION_MADE;
          m_LastStateChangeTime = m_CurrentTime;
        }
      }
      else {
        // Update the menu to keep it at unity and allow for closing animations
        m_radialMenu.InteractWithCursor(m_radialMenu.Translation());
        
        // VOLUME UPDATE
        float absRot = 0; // absolute rotation of the hand
        float offset = 0; // offset between menu start and current rotation
        int sign = 1;
        float visualNorm = 0;
        float norm = 0;
        float velocity = 0;
      
        const float DEADZONE = 0.3f;
        const float MAX = static_cast<float>(M_PI / 4.0);
        const float MAX_VELOCTY = 0.4f;
        
        if( !m_hasRoll ) {
          m_startRoll = dra.absoluteRoll;
          m_hasRoll = true;
          return;
        }
        
        absRot = dra.absoluteRoll;
        offset = absRot - m_startRoll;
        
        // Make sure offset represents the smallest representation of the offset angle.
        offset = fabs(offset) > M_PI ? static_cast<float>(2*M_PI - fabs(offset)) : offset;

        sign = offset < 0 ? -1 : 1; // Store the direction of the offset before we normalize it.
        visualNorm = fabs(offset) / MAX; // The normalization for the visual feedback doens't use the deadzone.
        visualNorm = std::min(1.0f, std::max(0.0f, visualNorm)); //Clamp the visual output
        norm = (fabs(offset) - DEADZONE) / (MAX - DEADZONE); // The normalization for the input has a deadzone.
        norm = std::min(1.0f, std::max(0.0f, norm)); //Clamp the normalized input
        
        // Rotate the volume knob in the view based on the user's normalized input.
        m_volumeKnob->LinearTransformation() = Eigen::AngleAxis<double>(visualNorm * (M_PI/2.0) * sign, Vector3::UnitZ()).toRotationMatrix();
        
        // Calcuate velocity from the normalized input value.
        velocity = norm * MAX_VELOCTY * sign * (frameTime.deltaTime / 100000.0f);
        
        // Send the velocity to the controller to update the system volume.
        m_mediaViewEventListener(&MediaViewEventListener::OnUserChangedVolume)(calculateVolumeDelta(velocity));
      }
      break;
    }
    case State::SELECTION_MADE:
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
    m_radialMenu.GetItem(i)->LocalProperties().AlphaMaskProperty().SetApplyType(apply_type);
  }
  m_radialMenu.UpdateItemActivation(static_cast<float>(1E-6 * frameTime.deltaTime));
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

void MediaViewStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {
  float alphaMask = 0.0f;
  if (m_state == State::ACTIVE) {
    // fade in
    alphaMask = SmootherStep(std::min(1.0f, static_cast<float>((m_CurrentTime - m_LastStateChangeTime)/m_FadeTime)));
  } else if (m_state == State::FADE_OUT || m_state == State::SELECTION_MADE || m_state == State::INACTIVE) {
    // fade out
    alphaMask = SmootherStep(1.0f-std::min(1.0f, static_cast<float>((m_CurrentTime - m_LastStateChangeTime)/m_FadeTime)));
    if (m_selectedItem >= 0) {
      const float itemOpacity = SmootherStep(1.0f-std::min(1.0f, static_cast<float>((m_CurrentTime - 2*m_FadeTime - m_LastStateChangeTime)/m_FadeTime)));
      m_radialMenu.GetItem(m_selectedItem)->LocalProperties().AlphaMask() = itemOpacity;
    } else {
      for (int i=0; i<numItems; i++) {
        m_radialMenu.GetItem(i)->LocalProperties().AlphaMask() = 1.0f;
      }
    }
  }
  m_radialMenu.LocalProperties().AlphaMask() = alphaMask;
  m_volumeSlider.LocalProperties().AlphaMask() = alphaMask;
}

void MediaViewStateMachine::Render(const RenderFrame &renderFrame) const  {
  if (m_state == State::ACTIVE || m_state == State::SELECTION_MADE || m_state == State::FADE_OUT) {
    PrimitiveBase::DrawSceneGraph(m_radialMenu, renderFrame.renderState);
    PrimitiveBase::DrawSceneGraph(m_volumeSlider, renderFrame.renderState);
    PrimitiveBase::DrawSceneGraph(*m_volumeKnob, renderFrame.renderState);
  }
}

//TODO: Filter this data in the recognizer to smooth things out.
float MediaViewStateMachine::calculateVolumeDelta(float deltaHandRoll) {
  return deltaHandRoll / static_cast<float>(3 * PI / 2.0);
}
