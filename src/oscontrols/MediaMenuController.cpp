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
void MediaMenuController::AutoFilter(const HandExistenceState &hes, const CursorMap& handScreenLocations) {
  if(!m_controllingHand.isValid()) { // If there is NOT a controlling hand
    if(!hes.m_newHands.empty()) {
      m_controllingHand = hes.m_newHands[0];
      m_mediaView->SetFadeState(MediaView::FADE_IN);
    }
  }
  else if(!isHandInVector(hes.m_stableHands, m_controllingHand)) { // There is no longer a controlling hand
    m_mediaView->SetFadeState(MediaView::FADE_OUT); 
    m_controllingHand = Leap::Hand::invalid();
  }
  else { // Update Logic when a hand is controlling the menu
    RenderEngineNode::Transform transform = m_mediaView->ComputeTransformToGlobalCoordinates();
    const auto positionRaw = transform.translation();
    const Vector2 position(positionRaw.x(),positionRaw.y());
    Vector2 userPosition;
    int selectedWedgeIndex = -1;
    
    // Figure out where the user's input is relative to the center of the menu
    auto q = handScreenLocations.find(m_controllingHand.id());
    if(q != handScreenLocations.end()) {
      userPosition = q->second;
    }
    else {
      throw std::runtime_error("no coords for controlling hand.");
      return;
    }
    
    float distance = (position - userPosition).norm();
    
    //Logic to perform depending on where the user's input is relative to the menu in terms of distance and screen position
    if(distance >= configs::MEDIA_MENU_CENTER_DEADZONE_RADIUS) { // Dragging a wedge out
      selectedWedgeIndex = m_mediaView->setActiveWedgeFromPoint(userPosition);
      m_mediaView->setInteractionDistance(distance);
      
      if( distance >= configs::MEDIA_MENU_ACTIVATION_RADIUS ) { // Making a selection
        closeMenu(true);
        //TODO: Hook up wedge event actions maybe with a switch statement
        switch(selectedWedgeIndex) {
          case 0:
            break;
          case 1:
            break;
          case 2:
            break;
          default:
            break;
        }
      }
    }
    else { // within the deadzone
      m_mediaView->deselectWedges();
    }
    
  }
}

void MediaMenuController::closeMenu(bool keepSelectionVisible) {
  m_mediaView->closeMenu(0.5);
}

bool MediaMenuController::isHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand) {
  bool retVal = false;
  for(Leap::Hand hand : vect) {
    if(hand.id() == goalHand.id()) {
      retVal = true;
    }
  }
  return retVal;
}



