#include "HandActivationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "InteractionConfigs.h"

HandActivationRecognizer::HandActivationRecognizer() :
  m_wasPinching(false),
  m_lastPinchStrength(0.0f),
  m_wasGrabbing(false),
  m_lastGrabStrength(0.0f)
{
}

void HandActivationRecognizer::AutoFilter(const Leap::Hand &hand, const FrameTime& frameTime, HandGrab& handGrab, HandPinch &handPinch) {
  
  handPinch.pinchStrength = getCustomPinchStrength(hand);
  handPinch.isPinching = m_wasPinching;
  
  handGrab.grabStrength = hand.grabStrength();
  handGrab.isGrabbing = m_wasGrabbing;
  
  if ( !m_wasPinching ) {
    if ( handPinch.pinchStrength > MIN_PINCH_START ) {
      handPinch.isPinching = true;
    }
  }
  else {
    if ( handPinch.pinchStrength < MIN_PINCH_CONTINUE ) {
      handPinch.isPinching = false;
    }
  }
  
  if ( !m_wasGrabbing ) {
    if ( handGrab.grabStrength > MIN_GRAB_START ) {
      handGrab.isGrabbing = true;
    }
  }
  else {
    if ( handGrab.grabStrength < MIN_GRAB_CONTINUE ) {
      handGrab.isGrabbing = false;
    }
  }
  
  handPinch.pinchDeltaPerSecond = (handPinch.pinchStrength - m_lastPinchStrength) / (frameTime.deltaTime / 1000000.0);
  handGrab.grabDeltaPerSecond = (handGrab.grabStrength - m_lastGrabStrength) / (frameTime.deltaTime / 1000000.0);
  
  m_wasPinching = handPinch.isPinching;
  m_wasGrabbing = handGrab.isGrabbing;
  m_lastPinchStrength = handPinch.pinchStrength;
  m_lastGrabStrength = handGrab.grabStrength;
}

float HandActivationRecognizer::getCustomPinchStrength(const Leap::Hand& hand) const {
  const float MAX_DISTANCE = 120.0f;
  const float MIN_DISTANCE = 20.0f;
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

