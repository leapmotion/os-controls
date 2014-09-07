#include "HandPinchRecognizer.h"
#include "HandPoseRecognizer.h"
#include "InteractionConfigs.h"

HandPinchRecognizer::HandPinchRecognizer() :
  m_wasPinching(false),
  m_lastPinchStrength(0.0f)
{
}

void HandPinchRecognizer::AutoFilter(const Leap::Hand &hand, const FrameTime& frameTime, HandPinch &handPinch) {
  
  handPinch.pinchStrength = hand.grabStrength();//getCustomPinchStrength(hand);
  handPinch.isPinching = m_wasPinching;
  if ( !m_wasPinching ) {
    if ( hand.pinchStrength() > config::MIN_PINCH_FOR_PINCHING ) {
      handPinch.isPinching = true;
    }
  }
  else {
    if ( hand.pinchStrength() < config::MAX_PINCH_FOR_NOT_PINCHING ) {
      handPinch.isPinching = false;
    }
  }
  
  handPinch.pinchDeltaPerSecond = (handPinch.pinchStrength - m_lastPinchStrength) / (frameTime.deltaTime / 1000000.0);

  m_wasPinching = handPinch.isPinching;
  m_lastPinchStrength = handPinch.pinchStrength;
}

float HandPinchRecognizer::getCustomPinchStrength(const Leap::Hand& hand) const {
  const float MAX_DISTANCE = 120.0f;
  const float MIN_DISTANCE = 5.0f;
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

