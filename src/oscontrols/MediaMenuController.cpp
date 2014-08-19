#include "stdafx.h"
#include "MediaMenuController.h"
#include "osControlConfigs.h"
#include "math.h"


MediaMenuController::MediaMenuController()
{
  m_mediaView = RenderEngineNode::Create<MediaView>(Vector3(300, 300, 0), 5.0f);
  m_rootNode->AddChild(m_mediaView);
}

//Update
void MediaMenuController::AutoFilter(const HandPointingMap &hpm, const CursorMap& handScreenLocations) {
  if(!m_controllingHand.isValid()) { // If there is NOT a controlling hand
    if(!hpm.empty()) {
      m_controllingHand = hpm.begin()->second;
      m_mediaView->SetFadeState(MediaView::FADE_IN);
      m_isInteractionComplete = false;
      
      Vector2 newPosition = handScreenLocations.at(m_controllingHand.id());
      m_mediaView->Move(Vector3(newPosition.x(), newPosition.y(), 0));
    }
  }
  else { // Update Logic when a hand is controlling the menu
    
    //Check if controlling hand is still pointing
    try {
      hpm.at(m_controllingHand.id());
    }
    catch (std::out_of_range e) {
      m_mediaView->SetFadeState(MediaView::FADE_OUT);
      m_controllingHand = Leap::Hand::invalid();
      return;
    }
    
    if ( !m_isInteractionComplete ) {
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
      
      float distance = (position - userPosition).norm();
      
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
  }
}

void MediaMenuController::closeMenu(bool keepSelectionVisible) {
  m_mediaView->CloseMenu(0.5);
}



