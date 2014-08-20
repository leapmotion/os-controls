#include "stdafx.h"
#include "MediaMenuController.h"
#include "osControlConfigs.h"
#include "math.h"


MediaMenuController::MediaMenuController() :
m_lastProgress(std::numeric_limits<float>::min()),
m_lastRoll(std::numeric_limits<float>::min())
{
  m_mediaView = RenderEngineNode::Create<MediaView>(Vector3(300, 300, 0), 5.0f);
  m_rootNode->AddChild(m_mediaView);
}

void MediaMenuController::AutoFilter(const HandPoseVector &hpv, const CursorMap& handScreenLocations, const GestureMap& handGestures, const RollMap& handRolls) {
  std::cout << "One Finger Extended: " << hpv[1].size() << std::endl;
  if(!m_controllingHand.isValid()) { // if there is NOT a controlling hand
    if(!hpv[1].empty()) {
      m_controllingHand = hpv[1].begin()->second;
      m_mediaView->FadeIn();

      m_mediaView->SetVolume(m_audioVolumeInterface->GetVolume());
      m_lastRoll = std::numeric_limits<float>::min();
      
      m_isInteractionComplete = false;
      
      Vector2 newPosition = handScreenLocations.at(m_controllingHand.id());
      m_mediaView->Move(Vector3(newPosition.x(), newPosition.y(), 0));
    }
  }
  else {
    //Check if controlling hand is still pointing
    try {
      hpv[1].at(m_controllingHand.id());
    }
    catch (std::out_of_range e) {
      m_mediaView->FadeOut();
      m_controllingHand = Leap::Hand::invalid();
      return;
    }
    
    // Update
    if ( !m_isInteractionComplete ) {
      updateVolumeControl(handRolls);
      updateWedges(handScreenLocations);
    }
  }
}

void MediaMenuController::updateVolumeControl(const RollMap& handRolls) {
  auto roll = handRolls.find(m_controllingHand.id())->second;

  if (m_lastRoll == std::numeric_limits<float>::min() || std::abs(roll - m_lastRoll) > 0.5f) {
    m_lastRoll = roll;
    return;
  }
  
  std::cout << "Roll: " << roll << std::endl;
  std::cout << "dRoll: " << roll - m_lastRoll << std::endl;
  
  m_mediaView->NudgeVolume((roll - m_lastRoll)/M_PI);
  m_audioVolumeInterface->SetVolume(m_mediaView->Volume());
  
  m_lastRoll = roll;
}

void MediaMenuController::updateWedges(const CursorMap& handScreenLocations) {
  RenderEngineNode::Transform transform = m_mediaView->ComputeTransformToGlobalCoordinates();
  const auto positionRaw = transform.translation();
  const Vector2 position(positionRaw.x(),positionRaw.y());
  Vector2 userPosition;
  int selectedWedgeIndex = -1;
  
  // Figure out where the user's input is relative to the center of the menu
  try {
    userPosition = handScreenLocations.at(m_controllingHand.id());
  }
  catch (std::out_of_range e) {
    throw std::runtime_error("no coords for controlling hand.");
    return;
  }
  
  float distance = static_cast<float>((position - userPosition).norm());
  
  selectedWedgeIndex = m_mediaView->setActiveWedgeFromPoint(userPosition);
  
  //Logic to perform depending on where the user's input is relative to the menu in terms of distance and screen position
  if(distance >= configs::MEDIA_MENU_CENTER_DEADZONE_RADIUS) { // Dragging a wedge out
    m_mediaView->SetInteractionDistance(distance - configs::MEDIA_MENU_CENTER_DEADZONE_RADIUS);
    
    if( distance >= configs::MEDIA_MENU_ACTIVATION_RADIUS ) { // Making a selection
      closeMenu(true);
      m_isInteractionComplete = true;
      //TODO: Hook up wedge event actions maybe with a switch statement
      switch(selectedWedgeIndex) {
        case 0:
          std::cout << "activate top" << std::endl;
          m_mediaInterface->PlayPause();
          break;
        case 1:
          std::cout << "activate right" << std::endl;
          m_mediaInterface->Next();
          break;
        case 3:
          std::cout << "activate left" << std::endl;
          m_mediaInterface->Prev();
          break;
        default:
          break;
      }
    }
  }
  else { // within the deadzone
    m_mediaView->DeselectWedges();
  }
}

void MediaMenuController::closeMenu(bool keepSelectionVisible) {
  m_mediaView->CloseMenu(0.5);
}



