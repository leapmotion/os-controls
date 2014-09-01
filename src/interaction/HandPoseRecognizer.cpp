//
//  HandPointingDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
//
#include "InteractionConfigs.h"
#include "HandPoseRecognizer.h"


void HandPoseRecognizer::AutoFilter(const Leap::Hand& hand, const HandPinch& handPinch, HandPose& handPose) {
  
  if ( !hand.isValid() ) {
    return;
  }
  
  /*if(handPinch.isPinching) {
    handPose = HandPose::ZeroFingers;
    return;
  }*/
  
  Vector3 handDirection = hand.direction().toVector3<Vector3>();
  
  int handCode = 0;
  int i = 0;
  
  for( auto finger : hand.fingers() ) {
    Vector3 fingerDirection = finger.direction().toVector3<Vector3>();
    float dot = handDirection.dot(fingerDirection); // How similar is the direciton of the hand and finger.
    
    //TODO: Fix pinkey finger extended and thumb extended.
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
  
  switch (handCode) {
    case 0:  //00000
      handPose = HandPose::ZeroFingers;
      break;
    case 8:  //01000
    case 24: //11000
      handPose = HandPose::OneFinger;
      break;
    case 12: //01100
      handPose = HandPose::TwoFingers;
      break;
    case 28: //11100
    case 14: //01110
      handPose = HandPose::ThreeFingers;
      break;
    case 30: //11110
    case 15: //01111
      handPose = HandPose::FourFingers;
      break;
    case 31: //11111
      handPose = HandPose::FiveFingers;
      break;
    default:
      break;
  }
}