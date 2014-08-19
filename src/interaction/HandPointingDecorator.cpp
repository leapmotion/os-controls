//
//  HandPointingDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
//
#include "InteractionConfigs.h"
#include "HandPointingDecorator.h"

void HandPointingDecorator::AutoFilter(Leap::Frame frame, HandPointingMap& pointingHandsMap) {
  for(auto hand : frame.hands()) {
    if ( isPointing(hand) ) {
      pointingHandsMap[hand.id()] = hand;
    }
  }
}

bool HandPointingDecorator::isPointing(Leap::Hand hand) const {
  bool retVal = true;
  if ( hand.isValid() ) {
    for( auto finger : hand.fingers() ) {
      Vector3 handDirection = hand.direction().toVector3<Vector3>();
      Vector3 fingerDirection = finger.direction().toVector3<Vector3>();
      float dot = handDirection.dot(fingerDirection); // How similar is the direciton of the hand and finger.
      
      if ( finger.type() == Leap::Finger::TYPE_INDEX) {
        if ( dot < config::MIN_DOT_FOR_POINTING ) { retVal = false; } // Not pointing if the index finger is not extended.
      }
      else if ( finger.type() == Leap::Finger::TYPE_THUMB ) {
        continue; // ignore the thumb
      }
      else {
        if (dot > config::MAX_DOT_FOR_CURLED ) { retVal = false; } // Not pointing if any non-thumb and non-index finger is extended.
      }
    }
  }
  return retVal;
}