//
//  HandPointingDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
#include "InteractionConfigs.h"
#include "HandPoseRecognizer.h"
#include "utility/CircleFitter.h"

HandPoseRecognizer::HandPoseRecognizer(void) :
m_lastPose(HandPose::ZeroFingers) {
  memset(lastExtended, 0, sizeof(lastExtended));
}


void HandPoseRecognizer::AutoFilter(const Leap::Hand& hand, HandPose& handPose) {
  
  if ( !hand.isValid() ) {
    return;
  }
  
  bool isClawCurling = true; //initial value
  bool isClawDistance = areTipsSeparated(hand);
  bool isPalmPointingDown = false;
  bool fingersOut = true;
  
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
      Vector3 palmPos = hand.palmPosition().toVector3<Vector3>();
      Vector3 tipPos = finger.tipPosition().toVector3<Vector3>();
      Vector3 diff = tipPos - palmPos;
      float zDist = fabs(diff.z());
      switch ( m_lastPose ) {
        case HandPose::Clawed:
          if ( zDist < persist_fingersForward ) {
            fingersOut = false;
          }
          break;
          
        default:
          if ( zDist < activate_fingersForward ) {
            fingersOut = false;
          }
          break;
      }
    }
    
    
    i++;
  }
  
  isClawDistance = areTipsSeparated(hand);
  
  float palmY = hand.palmNormal().toVector3<Vector3>().y();
  switch ( m_lastPose ) {
    case HandPose::Clawed:
      if ( palmY < persist_palmDown ) {
        isPalmPointingDown = true;
      }
      break;
    default:
      if ( palmY <= activate_palmDown ) {
        isPalmPointingDown = true;
      }
      break;
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
      handPose = HandPose::ZeroFingers;
      break;
  }
  
  std::cout << "fingersOut: " << fingersOut << std::endl;
  std::cout << "isClawCurling: " << isClawCurling << std::endl;
  std::cout << "isClawDistance: " << isClawDistance << std::endl;
  std::cout << "fingersOut: " << fingersOut << std::endl;
  std::cout << "NOT isPalmPointingDown: " << !isPalmPointingDown << std::endl;
  
  //TODO: add some curve detection to fingers to make
  //      this differentiated from a simple 3 finger or 4 finger pose.
  if (handPose != HandPose::OneFinger &&
      isClawCurling &&
      isClawDistance &&
      fingersOut &&
      !isPalmPointingDown) {
    handPose = HandPose::Clawed;
  }
  
  m_lastPose = handPose;
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

bool HandPoseRecognizer::isClawCurled(Leap::Finger finger) const {
  bool retVal = false;
  float averageBend = averageFingerBend(finger);
  std::cout << static_cast<int>(finger.type()) << " bend: " << averageBend << std::endl;
  if ( finger.type() != Leap::Finger::TYPE_THUMB ) {
    switch ( m_lastPose ) {
      case HandPose::Clawed:
        if ( averageBend > persist_clawCurl_min &&
            averageBend < persist_clawCurl_max ) {
          retVal = true;
        }
        break;
      default:
        if ( averageBend > activate_clawCurl_min &&
             averageBend < activate_clawCurl_max ) {
          retVal = true;
        }
        break;
    }
  }
  else {
    retVal = true;
  }
  return retVal;
}

bool HandPoseRecognizer::areTipsSeparated(Leap::Hand hand) const{
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
    switch ( m_lastPose ) {
      case HandPose::Clawed:
        if ( distance <=  persist_distance) {
          retVal = false;
        }
        break;
      default:
        if ( distance <=  activate_distance) {
          retVal = false;
        }
        break;
    }
  }
  
  return retVal;
}

float HandPoseRecognizer::averageFingerBend(Leap::Finger finger) const {
  float retVal = 0.0f;
  int count = 0;
  float sum = 0.0f;
  float average = 0.0f;
  
  int startBone = 3;
  
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

  retVal = average;
  return retVal;
}

float HandPoseRecognizer::projectAlongPalmNormal(Vector3 point, Leap::Hand hand) const {
  Vector3 diff = point - hand.palmPosition().toVector3<Vector3>();
  return static_cast<float>(diff.dot( hand.palmNormal().toVector3<Vector3>() ));
}
