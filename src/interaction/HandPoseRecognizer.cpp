//
//  HandPointingDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
#include "InteractionConfigs.h"
#include "HandPoseRecognizer.h"
#include "utility/CircleFitter.h"

HandPoseRecognizer::HandPoseRecognizer(void) {
  memset(lastExtended, 0, sizeof(lastExtended));
}


void HandPoseRecognizer::AutoFilter(const Leap::Hand& hand, HandPose& handPose) {
  
  if ( !hand.isValid() ) {
    return;
  }
  
  bool isClawCurling = true; //initial value
  bool isClawDistance = areTipsClawed(hand);
  bool isPalmPointingAtScreen = false;
  bool areFingersUp = true;
  
  int handCode = 0;
  int i = 0;
  
  for( auto finger : hand.fingers() ) {
    
    if ( isExtended(finger, lastExtended[i])) {
      handCode += (1 << (4-i));
      lastExtended[i] = true;
    }
    else {
      lastExtended[i] = false;
    }
    
    if (finger.type() == Leap::Finger::TYPE_THUMB ||
        finger.type() == Leap::Finger::TYPE_INDEX ||
        finger.type() == Leap::Finger::TYPE_MIDDLE) {
      if ( !isClawCurled(finger)) {
        isClawCurling = false;
      }
    }
    
    if (finger.type() == Leap::Finger::TYPE_INDEX ||
        finger.type() == Leap::Finger::TYPE_MIDDLE) {
      if ( !finger.isExtended() ) {
        areFingersUp = false;
      }
    }
    
    i++;
  }
  
  isClawDistance = areTipsClawed(hand);
  
  float palmZ = hand.palmNormal().toVector3<Vector3>().z();
  if ( palmZ < -0.05f ) {
    isPalmPointingAtScreen = true;
  }
  
  std::cout << "palmZ: " << palmZ << std::endl;
  std::cout << "isScreen: " << isPalmPointingAtScreen << std::endl;
  std::cout << "isDistance: " << isClawDistance << std::endl;
  std::cout << "fingersUp:  " << areFingersUp << std::endl;
  std::cout << "isCurved:   " << isClawCurling << std::endl << std::endl;
  
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
      handPose = HandPose::ZeroFingers;
      break;
  }
  
  //TODO: add some curve detection to fingers to make
  //      this differentiated from a simple 3 finger or 4 finger pose.
  if (isClawCurling &&
      isClawDistance &&
      isPalmPointingAtScreen &&
      areFingersUp) {
    handPose = HandPose::Clawed;
  }
}

//This could be cleaned up a lot to use some loops.
bool HandPoseRecognizer::isExtended(Leap::Finger finger, bool wasExtended) const {
  bool retVal = false;
  
  Leap::Bone metacarpal = finger.bone(Leap::Bone::TYPE_METACARPAL);
  Leap::Bone proximal = finger.bone(Leap::Bone::TYPE_PROXIMAL);
  Leap::Bone intermediate = finger.bone(Leap::Bone::TYPE_INTERMEDIATE);
  Leap::Bone distal = finger.bone(Leap::Bone::TYPE_DISTAL);
  
  float mToPDot = 1.0f;
  
  if ( finger.type() != Leap::Finger::TYPE_THUMB ) {
    mToPDot = metacarpal.direction().toVector3<Vector3>().dot(proximal.direction().toVector3<Vector3>());
  }
  float pToIDot = proximal.direction().toVector3<Vector3>().dot(intermediate.direction().toVector3<Vector3>());
  float iToDDot = intermediate.direction().toVector3<Vector3>().dot(distal.direction().toVector3<Vector3>());
  
  if ( !wasExtended ) {
    if(mToPDot >= config::MIN_DOT_FOR_START_POINTING && pToIDot >= config::MIN_DOT_FOR_START_POINTING && iToDDot >= config::MIN_DOT_FOR_START_POINTING) {
      retVal = true;
    }
  }
  else {
    if(mToPDot >= config::MAX_DOT_FOR_CONTINUE_POINTING &&pToIDot >= config::MAX_DOT_FOR_CONTINUE_POINTING && iToDDot >= config::MAX_DOT_FOR_CONTINUE_POINTING ) {
      retVal = true;
    }
  }

  return retVal;
}

bool HandPoseRecognizer::isNotDown(Leap::Finger finger) const {
  bool retVal = false;
  float yComponent = finger.bone(static_cast<Leap::Bone::Type>(1)).direction().toVector3<Vector3>().y();
  std::cout << static_cast<int>(finger.type()) << "yComponent: " << yComponent << std::endl;
  if ( yComponent < 0.0 ) {
    retVal = true;
  }
  
  return retVal;
}

bool HandPoseRecognizer::isClawCurled(Leap::Finger finger) const {
  bool retVal = false;
  float averageBend = averageFingerBend(finger);
  if ( finger.type() != Leap::Finger::TYPE_THUMB ) {
    if ( averageBend > 0.25f &&
         averageBend < 2.0f ) {
      retVal = true;
    }
  }
  else {
    if ( averageBend < 1.0 ) {
      retVal = true;
    }
  }
  return retVal;
}

bool HandPoseRecognizer::areTipsClawed(Leap::Hand hand) const{
  bool retVal = true;
  
  std::vector<Leap::Finger> clawFingers;
  
  for ( auto finger : hand.fingers() ) {
    if (finger.type() == Leap::Finger::TYPE_THUMB ||
        finger.type() == Leap::Finger::TYPE_INDEX ||
        finger.type() == Leap::Finger::TYPE_MIDDLE) {
      clawFingers.push_back(finger);
    }
  }
  
  for (int i=1; i < clawFingers.size(); i++) {
    double distance = (clawFingers[i].tipPosition().toVector3<Vector3>() - clawFingers[i-1].tipPosition().toVector3<Vector3>()).norm();
    if ( distance <= config::MIN_DISTANCE_FOR_CLAW ) {
      retVal = false;
    }
  }
  
  return retVal;
}

float HandPoseRecognizer::averageFingerBend(Leap::Finger finger) const {
  float retVal = 0.0f;
  int count = 0;
  float sum = 0.0f;
  float average = 0.0f;
  
  int startBone = 2;
  
  if ( finger.type() == Leap::Finger::TYPE_THUMB ) {
    startBone = 3;
  }
  
  for(int i=startBone; i<4; i++) {
    //Angle from scalar product
    Vector3 v1 = finger.bone(static_cast<Leap::Bone::Type>(i-1)).direction().toVector3<Vector3>();
    Vector3 v2 = finger.bone(static_cast<Leap::Bone::Type>(i)).direction().toVector3<Vector3>();
    double dot = v1.dot(v2);
    double theta = std::acos(dot);
    sum += static_cast<float>(theta);
    count++;
  }
  average = count > 0 ? sum / count : 0.0f;
  
  std::cout << static_cast<int>(finger.type()) << " average bend: " << average << std::endl;

  retVal = average;
  return retVal;
}
