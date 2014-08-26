#include "stdafx.h"
#include "MediaView.h"
#include "RenderFrame.h"
#include "RenderState.h"
#include "limits.h"
#include "uievents/osControlConfigs.h"
#include "uievents/OSCDomain.h"
#include <iostream>

const static float PI = 3.14159265f;

MediaView::MediaView(const Vector3& center, float offset) :
  m_state(State::INACTIVE),
  m_deadZone(true),
  m_opacity(0.0f, 0.2, EasingFunctions::QuadInOut<float>)
{
  //TODO: Move this into a for loop that handles the sweep angle calculations
  m_wedges[0] = RenderEngineNode::Create<PlayPauseWedge>(50 - offset, 100 - offset, 5 * PI / 4, 7 * PI / 4, Vector3(0, -1 * offset, 0)); //top
  m_wedges[1] = RenderEngineNode::Create<NextWedge>(50 - offset, 100 - offset, -PI / 4, PI / 4, Vector3(offset, 0, 0)); //right
  m_wedges[2] = RenderEngineNode::Create<Wedge>(50 - offset, 100 - offset, PI / 4, 3 * PI / 4, Vector3(0, offset, 0), true); //down
  m_wedges[3] = RenderEngineNode::Create<PrevWedge>(50 - offset, 100 - offset, 3 * PI / 4, 5 * PI / 4, Vector3(-1 * offset, 0, 0));  //left
  
  m_volumeControl = RenderEngineNode::Create<VolumeControl>(50 - 10 - offset, 10.0f);

  Translation() = center;
}

MediaView::~MediaView() {
  RemoveFromParent();
}

void MediaView::AutoInit() {
  auto self = shared_from_this();
  m_rootNode->AddChild(self);
  
  for(std::shared_ptr<RadialButton> radial : m_wedges) {
    AddChild(radial);
  }
  
  AddChild(m_volumeControl);
}

void MediaView::AnimationUpdate(const RenderFrame& frame) {
  m_opacity.Update(frame.deltaT.count());
  setMenuOpacity(m_opacity.Current());
}

void MediaView::Move(const Vector3& coords) {
  Translation() = coords;
}

void MediaView::OpenMenu(const HandLocation& handLocation) {
  // Update our position based on wherever the heck the hand is right now
  Move(Vector3(handLocation.x, handLocation.y, 0));
  
  fadeIn();
}

void MediaView::CloseMenu() {
  fadeOut();
}

bool MediaView::IsVisible() {
  return m_opacity.Current() > 0;
}

void MediaView::SetVolumeView(float volume) {
  m_volumeControl->SetVolume(volume);
}

void MediaView::NudgeVolumeView(float dVolume) {
  m_volumeControl->NudgeVolume(dVolume);
}

void MediaView::AutoFilter(OSCState appState, const Leap::Frame& frame, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll) {
  // State Transitions
  
  std::cout << "Media View State: " << static_cast<int>(m_state) << std::endl;
  
  if (appState == OSCState::FINAL && m_state != State::FINAL) {
    m_state = State::FINAL;
    CloseMenu();
    return;
  }
  
  switch( m_state )
  {
  case State::INACTIVE:
    if(appState == OSCState::MEDIA_MENU_FOCUSED) {
      resetWedges();
      OpenMenu(handLocation);
      m_state = State::ACTIVE;
    }
    break;
  case State::ACTIVE:
    if(appState != OSCState::MEDIA_MENU_FOCUSED) {
      CloseMenu();
      m_state = State::INACTIVE;
    }
    break;
  case State::SELECTION_MADE:
    CloseMenu();
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
    updateWedges(handLocation);
    m_mediaViewEventListener(&MediaViewEventListener::OnUserChangedVolume)(calculateVolumeDelta(dHandRoll.dTheta));
    break;
  case State::SELECTION_MADE:
    //something
    break;
  case State::FINAL:
  default:
    break;
  }

}

void MediaView::setMenuOpacity(float opacity) {
  m_wedges[0]->SetMaxOpacity(opacity);
  m_wedges[1]->SetMaxOpacity(opacity);
  m_wedges[3]->SetMaxOpacity(opacity);
  m_volumeControl->SetOpacity(opacity);
}

void MediaView::updateWedges(const HandLocation& handLocation) {
  std::shared_ptr<Wedge> activeWedge = closestWedgeToPoint(handLocation);
  float distanceFromDeadzone = distanceFromCenter(handLocation) - configs::MEDIA_MENU_CENTER_DEADZONE_RADIUS;
  
  //Logic to perform depending on where the user's input is relative to the menu in terms of distance and screen position
  if(distanceFromDeadzone < 0) {
    // Distance too short to do anything, return here
    m_deadZone = true;
    return;
  } else {
    m_deadZone = false;
  }
  
  updateWedgePositions(activeWedge, distanceFromDeadzone);
  checkForSelection(activeWedge, distanceFromDeadzone);
  
  m_lastActiveWedge = activeWedge;
}

float MediaView::distanceFromCenter(const HandLocation& handLocation) {
  RenderEngineNode::Transform transform = ComputeTransformToGlobalCoordinates();
  const auto positionRaw = transform.translation();
  const Vector2 position(positionRaw.x(), positionRaw.y());
  
  // Figure out where the user's input is relative to the center of the menu
  return static_cast<float>((position - handLocation.screenPosition()).norm());
}

std::shared_ptr<Wedge> MediaView::closestWedgeToPoint(const HandLocation& handLocation) {
  std::shared_ptr<Wedge> closestWedge = m_wedges[0];
  float minDist = std::numeric_limits<float>::max();
  
  for(auto wedge : m_wedges) {
    float dist = wedge->DistanceToCenter(handLocation.screenPosition());
    if ( dist < minDist ) {
      minDist = dist;
      closestWedge = wedge;
    }
  }
  
  return closestWedge; // return the index of the selected wedge.
}

void MediaView::updateWedgePositions(std::shared_ptr<Wedge> activeWedge, float distanceFromDeadzone) {
  for(auto wedge : m_wedges) {
    //TODO: Wedge Coloring and Such
    if ( activeWedge == wedge ) {
      wedge->Nudge(distanceFromDeadzone);
    }
    else {
      wedge->Nudge(0);
    }
  }
}

void MediaView::checkForSelection(std::shared_ptr<Wedge> activeWedge, float distanceFromDeadzone) {
  if(distanceFromDeadzone >= configs::MEDIA_MENU_ACTIVATION_RADIUS) { // Making a selection
    activeWedge->OnSelected();
    m_state = State::SELECTION_MADE;
  }
}

void MediaView::resetWedges() {
  for(auto wedge : m_wedges) {
    wedge->Nudge(0);
  }
}

float MediaView::calculateVolumeDelta(float deltaHandRoll) {
  return deltaHandRoll / (3 * PI / 2.0);
}
