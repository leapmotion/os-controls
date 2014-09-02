#include "HandPinchRecognizer.h"
#include "HandPoseRecognizer.h"
#include "InteractionConfigs.h"

HandPinchRecognizer::HandPinchRecognizer() :
wasPinching(false)
{
}

void HandPinchRecognizer::AutoFilter(const Leap::Hand &hand, const HandPose& handPose, HandPinch &handPinch) {
  
  handPinch.pinchStrength = hand.pinchStrength();
  handPinch.isPinching = wasPinching;
  if ( !wasPinching ) {
    
    if ( handPose != HandPose::OneFinger && handPose != HandPose::TwoFingers ) {
      if ( hand.pinchStrength() > config::MIN_PINCH_FOR_PINCHING ) {
        handPinch.isPinching = true;
      }
    }
  }
  else {
    if ( hand.pinchStrength() < config::MAX_PINCH_FOR_NOT_PINCHING ) {
      handPinch.isPinching = false;
    }
  }

  wasPinching = handPinch.isPinching;
}