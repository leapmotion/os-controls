#include "stdafx.h"
#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "CoordinateUtility.h"
#include "InteractionConfigs.h"

HandLocationRecognizer::HandLocationRecognizer(void) :
isInitialized(false)
{}
HandLocationRecognizer::~HandLocationRecognizer(void) {}

void HandLocationRecognizer::AutoFilter(const Leap::Hand& hand, const HandPose& handPose, HandLocation& handLocation) {
  Vector2 screenLocation;
  
  //We sometimes want to offset the palm position based on the direction the user is pointing
  Vector3 offset(0,0,0);
  
  screenLocation = m_coordinateUtility->LeapToScreen(hand.palmPosition());

  for( auto finger : hand.fingers() ) {
    if ( finger.type() == Leap::Finger::TYPE_INDEX) {
      offset = finger.direction().toVector3<Vector3>();
    }
  }
  
  offset *= config::OFFSET_DISTANCE;
  
  screenLocation += ProjectVector(2, offset);
  
  screenLocation.y() *= -1;
  handLocation.x = screenLocation.x();
  handLocation.y = screenLocation.y();
  
  if ( isInitialized ) {
    handLocation.dX = handLocation.x - lastPosition.x();
    handLocation.dY = handLocation.y - lastPosition.y();
  } else {
    handLocation.dX = 0;
    handLocation.dY = 0;
  }
  
  lastPosition = handLocation.screenPosition();
  isInitialized = true;
}