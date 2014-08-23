#include "stdafx.h"
#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "CoordinateUtility.h"
#include "InteractionConfigs.h"

HandLocationRecognizer::HandLocationRecognizer(void) {}
HandLocationRecognizer::~HandLocationRecognizer(void) {}

void HandLocationRecognizer::AutoFilter(const Leap::Hand& hand, const HandPose& handPose, HandLocation& handLocation) {
  Vector2 screenLocation;
  
  screenLocation = m_coordinateUtility->LeapToScreen(hand.palmPosition());

  for ( auto finger : hand.fingers() ) {
    if ( finger.type() == Leap::Finger::TYPE_INDEX ) {
      Vector3 fingerDirection = finger.direction().toVector3<Vector3>();
      fingerDirection *= config::FINGER_OFFSET_DISTANCE;
      Vector2 direction = ProjectVector(2, finger.direction().toVector3<Vector3>());
      screenLocation += direction;
    }
  }
  
  screenLocation.y() *= -1;
  handLocation.x = screenLocation.x();
  handLocation.y = screenLocation.y();
}