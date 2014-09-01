#include "HandPinchRecognizer.h"
#include "InteractionConfigs.h"

HandPinchRecognizer::HandPinchRecognizer() :
wasPinching(false)
{
}

void HandPinchRecognizer::AutoFilter(const Leap::Hand &hand, HandPinch &handPinch) {
  float pinch = hand.pinchStrength();
  handPinch.pinchStrength = hand.pinchStrength();
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