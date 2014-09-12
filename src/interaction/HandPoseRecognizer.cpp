#include "InteractionConfigs.h"
#include "HandPoseRecognizer.h"
#include "utility/CircleFitter.h"

HandPoseRecognizer::HandPoseRecognizer(void) :
m_lastPose(HandPose::ZeroFingers) {
  memset(lastExtended, 0, sizeof(lastExtended));
  
  lastPosition.setZero();
}


void HandPoseRecognizer::AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, const HandPinch& handPinch, HandPose& handPose) {
  
  if ( !hand.isValid() ) {
    return;
  }
  
  int handCode = 0; // for extention based pose recognition.
  
  int i = 0; // index tracker for for loop.
  
  //Checks against each finger
  for( auto finger : hand.fingers() ) {
    
    // Check if fingers are extended
    if ( isExtended(finger, lastExtended[i])) {
      handCode += (1 << (4-i));
      lastExtended[i] = true;
    }
    else {
      lastExtended[i] = false;
    }
    
    i++;
  }
    
  // Finger-Extension based pose resolution
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
  
  //Store the previous hand pose.
  m_lastPose = handPose;
}

bool HandPoseRecognizer::isExtended(Leap::Finger finger, bool wasExtended) const {
  bool retVal = false;
  float bend = 0.0f;
  
  if ( finger.type() == Leap::Finger::TYPE_THUMB ) {
    bend = averageFingerBend(finger, 2, 4);
    if ( !wasExtended ) {
      if(bend <= pointingConfigs::MAX_BEND_FOR_THUMB_START_POINTING) {
        retVal = true;
      }
    }
    else {
      if(bend <= pointingConfigs::MAX_BEND_FOR_THUMB_CONTINUE_POINTING) {
        retVal = true;
      }
    }
    return retVal;
  }

  bend = metaToDistalBend(finger);
  
  if ( !wasExtended ) {
    if(bend <= pointingConfigs::MAX_BEND_FOR_START_POINTING) {
      retVal = true;
    }
  }
  else {
    if(bend <= pointingConfigs::MAX_BEND_FOR_CONTINUE_POINTING) {
      retVal = true;
    }
  }

  return retVal;
}

float HandPoseRecognizer::metaToDistalBend(Leap::Finger finger) const {
  Leap::Bone distal = finger.bone(Leap::Bone::TYPE_METACARPAL);
  Leap::Bone metacarpal = finger.bone(Leap::Bone::TYPE_DISTAL);
  
  Vector3 v1 = metacarpal.direction().toVector3<Vector3>();
  Vector3 v2 = distal.direction().toVector3<Vector3>();
  
  double dot = v1.dot(v2);
  double theta = std::acos(dot);
  return theta;
}

float HandPoseRecognizer::averageFingerBend(Leap::Finger finger, int startBone, int endBone) const {
  float retVal = 0.0f;
  int count = 0;
  float sum = 0.0f;
  float average = 0.0f;
  startBone = std::max(1, startBone);
  endBone = std::min(4, endBone);
  
  for(int i=startBone; i<endBone; i++) {
    //Angle from scalar product
    Vector3 v1 = finger.bone(static_cast<Leap::Bone::Type>(i-1)).direction().toVector3<Vector3>();
    Vector3 v2 = finger.bone(static_cast<Leap::Bone::Type>(i)).direction().toVector3<Vector3>();
    double dot = v1.dot(v2);
    double theta = std::acos(dot);
    sum += static_cast<float>(theta);
    count++;
  }
  average = count > 0 ? sum / count : 0.0f;
  
  retVal = average;
  return retVal;
}