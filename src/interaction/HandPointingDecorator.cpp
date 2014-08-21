//
//  HandPointingDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
//
#include "InteractionConfigs.h"
#include "HandPointingDecorator.h"

void HandPointingDecorator::AutoFilter(const Leap::Hand& hand, HandPoses& handPose) {
  const static std::map<int, int> handCodes = {{0, 0}, {8, 1}, {24, 1}, {12, 2}, {28, 3}, {14, 3}, {30, 4}, {15, 4}, {31, 5}};
  
  if ( !hand.isValid() ) {
    return;
  }
  
  Vector3 handDirection = hand.direction().toVector3<Vector3>();
  
  int handCode = 0;
  int i = 0;
  
  for( auto finger : hand.fingers() ) {
    Vector3 fingerDirection = finger.direction().toVector3<Vector3>();
    float dot = handDirection.dot(fingerDirection); // How similar is the direciton of the hand and finger.
    
    if (finger.type() == Leap::Finger::TYPE_THUMB) {
      if (dot < config::MAX_DOT_FOR_THUMB_POINTING) {
        handCode += (1 << (4-i));
      }
    }
    else {
      if ( dot > config::MIN_DOT_FOR_POINTING ) {
        handCode += (1 << (4-i));
      }
    }

    
    i++;
  }
}