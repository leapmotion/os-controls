#include "HandPinchRecognizer.h"
#include "HandPoseRecognizer.h"
#include "InteractionConfigs.h"

HandPinchRecognizer::HandPinchRecognizer() :
wasPinching(false)
{
}

void HandPinchRecognizer::AutoFilter(const Leap::Hand &hand, HandPinch &handPinch) {
  
  handPinch.pinchStrength = getCustomPinchStrength(hand);
  std::cout << "pinch strength: " << handPinch.pinchStrength << std::endl;
  handPinch.isPinching = wasPinching;
  if ( !wasPinching ) {
    if ( hand.pinchStrength() > config::MIN_PINCH_FOR_PINCHING ) {
      handPinch.isPinching = true;
    }
  }
  else {
    if ( hand.pinchStrength() < config::MAX_PINCH_FOR_NOT_PINCHING ) {
      handPinch.isPinching = false;
    }
  }

  wasPinching = handPinch.isPinching;
}

float HandPinchRecognizer::getCustomPinchStrength(const Leap::Hand& hand) const {
  const float MAX_DISTANCE = 50.0f;
  const float MIN_DISTANCE = 10.0f;
  float retVal = 0.0f;
  Leap::Finger index = Leap::Finger::invalid();
  Leap::Finger thumb = Leap::Finger::invalid();
  
  for( auto finger : hand.fingers() ) {
    if ( finger.type() == Leap::Finger::TYPE_INDEX ) {
      index = finger;
    }
    else if ( finger.type() == Leap::Finger::TYPE_THUMB ) {
      thumb = finger;
    }
  }
  
  if ( index != Leap::Finger::invalid() && thumb != Leap::Finger::invalid() ) {
    float distance = (index.tipPosition().toVector3<Vector3>() - thumb.tipPosition().toVector3<Vector3>()).norm();
    float norm = (distance - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE);
    norm = std::min(1.0f, std::max(0.0f, norm));
    norm = 1 - norm;
    retVal = norm;
  }
  
  return retVal;
}

