#include "stdafx.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderFrame.h"
#include "MediaViewStateMachine.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/oscDomain.h"
#include "uievents/osControlConfigs.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTexture2.h"
#include "TextFile.h"
#include "RenderState.h"
#include "Resource.h"
#include <memory>

const static float PI = 3.14159265f;

const double startAngle = 3 * M_PI/4.0;
const double endAngle = startAngle + 6 * (M_PI/4.0);
const int numItems = 3;
const Color bgColor(0.4f, 0.425f, 0.45f, 0.75f);
const Color fillColor(0.4f, 0.425f, 0.45f, 0.75f);
const Color handleColor(0.65f, 0.675f, 0.7f, 1.0f);
const Color handleOutlineColor(0.505f, 0.831f, 0.114f, 0.75f);

MediaViewStateMachine::MediaViewStateMachine() :
  m_radialMenu(new RadialMenu()),
  m_volumeSlider(new VolumeSliderView()),
  m_ghostCursor(new Disk()),
  m_lastHandPose(HandPose::ZeroFingers),
  m_state(State::ARMED),
  m_cursorBufferzoneOffset(0,0,0),
  m_selectedItem(-1),
  m_FadeTime(0.25),
  m_CurrentTime(0.0),
  m_LastStateChangeTime(0.0),
  m_interactionIsVolumeLocked(false)
{
  // Initialize Smoothed Values
  m_ghostCursorAlpha.SetInitialValue(0.0f);
  m_ghostCursorAlpha.SetSmoothStrength(0.6f);
  
  m_volumeViewAlpha.SetInitialValue(0.0f);
  m_volumeViewAlpha.SetSmoothStrength(0.6f);
  
  // Initialize Ghost Curosr
  m_ghostCursor->SetRadius(20.0f);
  m_ghostCursor->Material().SetDiffuseLightColor(GHOST_CURSOR_COLOR);
  m_ghostCursor->Material().SetAmbientLightColor(GHOST_CURSOR_COLOR);
  m_ghostCursor->Material().SetAmbientLightingProportion(1.0f);

  // Radial Menu Initialization
  m_radialMenu->SetStartAngle(startAngle);
  m_radialMenu->SetEndAngle(endAngle);
  m_radialMenu->SetNumItems(numItems);
  m_radialMenu->SetRadius(MENU_RADIUS);
  m_radialMenu->SetThickness(MENU_THICKNESS);
  
  for (int i=0; i<numItems; i++) {
    std::shared_ptr<RadialMenuItem>& item = m_radialMenu->GetItem(i);
    item->SetRadius(MENU_RADIUS);
    item->SetThickness(MENU_THICKNESS);
    item->SetActivatedRadius(ACTIVATION_RADIUS);
    item->Material().SetAmbientLightColor(bgColor);
    item->Material().SetAmbientLightingProportion(1.0f);
    item->Material().SetDiffuseLightColor(bgColor);
    item->SetHoverColor(fillColor);
    item->SetActivatedColor(handleOutlineColor);
  }
  
  // Setup SVGs for Radial Icons
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
  
  // Initilize Volume Slider
  m_volumeSlider->SetWidth(220.0f);
  m_volumeSlider->SetHeight(10.0f);
}

void MediaViewStateMachine::AutoInit() {
  m_rootNode->Add(shared_from_this());
}

void MediaViewStateMachine::AutoFilter(OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  const Vector2 menuOffset = m_radialMenu->Translation().head<2>();
  
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
        m_cursorBufferzoneOffset = calculateBufferZoneOffset(handData.locationData.screenPosition());
        m_cursorView->EnableLocationOverride();
        m_radialMenu->Translation() = Vector3(handData.locationData.x, handData.locationData.y, 0.0) + m_cursorBufferzoneOffset;
        m_volumeSlider->Translation() = m_radialMenu->Translation() + VOLUME_SLIDER_OFFSET;
        m_mediaViewEventListener(&MediaViewEventListener::OnInitializeVolume)();
        m_volumeViewAlpha.SetGoal(1.0f);
        m_state = State::ACTIVE;
        m_LastStateChangeTime = m_CurrentTime;
        return;
      }
      break;
    case State::ACTIVE:
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_state = State::ARMED;
        m_cursorView->DisableLocationOverride();
        m_cursorBufferzoneOffset = Vector3(0,0,0);
        doMenuUpdate(Vector2(0.0f,0.0f), menuOffset);
        resetMemberState();
        m_volumeViewAlpha.SetGoal(0.0f);
        m_LastStateChangeTime = m_CurrentTime;
        return;
      }
      break;
    case State::COMPLETE:
      
      if(appState != OSCState::MEDIA_MENU_FOCUSED) {
        m_cursorView->DisableLocationOverride();
        m_cursorBufferzoneOffset = Vector3(0,0,0);
        m_volumeViewAlpha.SetGoal(0.0f);
        m_state = State::ARMED;
        m_LastStateChangeTime = m_CurrentTime;
        return;
      }
      break;
    case State::FINAL:
    default:
      break;
  }

  // State Loops
  switch (m_state) {
    case State::ARMED:
      // Make sure menu is "reset"
      doMenuUpdate(Vector2(0.0f,0.0f), Vector2(0,0));
      // Wedge transparency is updated in AnimationUpdate loops
      m_radialMenu->InteractWithoutCursor();
      m_selectedItem = -1;
      break;
    case State::ACTIVE:
    {
      // The menu always thinks it's at (0,0) so we need to offset the cursor
      // coordinates by the location of the menu to give the proper space.
      if ( !m_interactionIsVolumeLocked ) {
        doMenuUpdate(handData.locationData.screenPosition() + ProjectVector(2, m_cursorBufferzoneOffset), menuOffset);
      }
      doVolumeUpdate(handData.locationData.screenPosition() + ProjectVector(2, m_cursorBufferzoneOffset), Vector2(handData.locationData.dX, handData.locationData.dY), menuOffset);
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

void MediaViewStateMachine::doMenuUpdate(const Vector2& locationData, Vector2 menuOffset) {
  Vector3 leapPosition(locationData.x() - menuOffset.x(), locationData.y() - menuOffset.y(), 0);
  RadialMenu::UpdateResult updateResult = m_radialMenu->InteractWithCursor(leapPosition);
  m_selectedItem = updateResult.updateIdx;
  if(updateResult.curActivation >= 0.95) { // the component doesn't always return a 1.0 activation. Not 100% sure why.
    //Selection Made Transition
    resolveSelection(updateResult.updateIdx);
    m_volumeViewAlpha.SetGoal(0.0f);
    m_cursorView->DisableLocationOverride();
    m_cursorBufferzoneOffset = Vector3(0,0,0);
    m_state = State::COMPLETE;
    m_LastStateChangeTime = m_CurrentTime;
  }
}

void MediaViewStateMachine::doVolumeUpdate(const Vector2& locationData, const Vector2& deltaPixels, Vector2 menuOffset) {
  const float VOLUME_OFFSET_START_Y = 80.0f;
  const float VOLUME_LOCK_IN_Y = 180.0f;
  const float VOLUME_LOCK_X_OFFSET = 35.0f;
  
  Vector3 leapPosition(locationData.x() - menuOffset.x(), locationData.y() - menuOffset.y(), 0);
  
  double offsetNormalFactor = (leapPosition.y() - VOLUME_OFFSET_START_Y) / (VOLUME_LOCK_IN_Y - VOLUME_OFFSET_START_Y);
  offsetNormalFactor = std::max(0.0, std::min(1.0, offsetNormalFactor));
  Vector2 cursorCalculatedPosition = m_cursorView->GetCalculatedLocation();
  cursorCalculatedPosition += ProjectVector(2, m_cursorBufferzoneOffset);
  if ( offsetNormalFactor > 0.0 ) {
    m_interactionIsVolumeLocked = true;
    m_ghostCursorAlpha.SetGoal(GHOST_CURSOR_ALPHA);
    Vector2 goalPosition = Vector2(m_volumeSlider->Translation().x() + m_volumeSlider->GetNotchOffset().x() + VOLUME_LOCK_X_OFFSET, std::min(static_cast<float>(cursorCalculatedPosition.y()), static_cast<float>(m_radialMenu->Translation().y() + VOLUME_LOCK_IN_Y)));
    Vector2 offsetCursorPosition = cursorCalculatedPosition + (offsetNormalFactor * (goalPosition - cursorCalculatedPosition));
    m_cursorView->position = OSVector2{ static_cast<float>(offsetCursorPosition.x()), static_cast<float>(offsetCursorPosition.y()) };
    m_ghostCursor->Translation() = Vector3(cursorCalculatedPosition.x(), cursorCalculatedPosition.y(), 0.0f);
    
    if ( offsetNormalFactor >= 1.0 ) {
      float deltaPixelsInVolume = deltaPixels.x() / m_volumeSlider->Width();
      m_volumeSlider->NudgeVolumeLevel(deltaPixelsInVolume);
      m_volumeSlider->Activate();
    }
    else {
      m_volumeSlider->Deactivate();
    }
  }
  else
  {
    m_cursorView->position = OSVector2{ static_cast<float>(cursorCalculatedPosition.x()), static_cast<float>(cursorCalculatedPosition.y()) };
    m_volumeSlider->Deactivate();
    m_ghostCursorAlpha.SetGoal(0.0f);
  }
  
  if ( leapPosition.norm() <=  MENU_RADIUS - (MENU_THICKNESS / 2.0f)) {
    m_interactionIsVolumeLocked = false;
  }
}

void MediaViewStateMachine::SetViewVolume(float volume) {
  m_volumeSlider->SetViewVolume(volume);
}

void MediaViewStateMachine::AnimationUpdate(const RenderFrame &renderFrame) {
  //Upate volume slider
  m_volumeSlider->Update(renderFrame);
  
  // Smoothed Value Updates
  m_ghostCursorAlpha.Update(static_cast<float>(renderFrame.deltaT.count()));
  m_volumeViewAlpha.Update(static_cast<float>(renderFrame.deltaT.count()));
  
  m_ghostCursor->LocalProperties().AlphaMask() = m_ghostCursorAlpha;
  m_volumeSlider->SetOpacity(m_volumeViewAlpha);
  
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
      PrimitiveBase::DrawSceneGraph(*m_ghostCursor, renderFrame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_volumeSlider, renderFrame.renderState);
    }
    else {
    }
  }
}

void MediaViewStateMachine::resetMemberState() {
  m_ghostCursorAlpha.SetGoal(0.0f);
  m_selectedItem = -1;
  m_interactionIsVolumeLocked = false;
}

//TODO: Filter this data in the recognizer to smooth things out.
float MediaViewStateMachine::calculateVolumeDelta(float deltaHandRoll) {
  return deltaHandRoll / static_cast<float>(3 * PI / 2.0f);
}

Vector3 MediaViewStateMachine::calculateBufferZoneOffset(const Vector2& screenPosition) {
  Vector3 retVal(0,0,0);
  
  // Find our distance from the screen edge
  float xEdgeDistance = std::min(screenPosition.x(), m_renderWindow->getSize().x - screenPosition.x());
  float yEdgeDistance = std::min(screenPosition.y(), m_renderWindow->getSize().y - screenPosition.y());
  
  // Calculate any offset needed
  float xOffset = std::max(0.0f, mediaMenuConfigs::SCREEN_EDGE_BUFFER_DISTANCE - xEdgeDistance);
  float yOffset = std::max(0.0f, mediaMenuConfigs::SCREEN_EDGE_BUFFER_DISTANCE - yEdgeDistance);
  
  // Make sure the offset is in the proper direction based on cloest edge
  xOffset = (screenPosition.x() > m_renderWindow->getSize().x / 2.0) ? xOffset*-1 : xOffset;
  yOffset = (screenPosition.y() > m_renderWindow->getSize().y / 2.0) ? yOffset*-1 : yOffset;
  
  retVal = Vector3( xOffset, yOffset, 0.0f );
  
  return retVal;
}
