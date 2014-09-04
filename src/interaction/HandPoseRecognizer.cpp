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
    
    if ( !isClawCurled(finger)) {
      isClawCurling = false;
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
      handPose = HandPose::ZeroFingers;
      break;
  }
  
  //TODO: add some curve detection to fingers to make
  //      this differentiated from a simple 3 finger or 4 finger pose.
  if (static_cast<int>(handPose) == 3) {
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

bool HandPoseRecognizer::isClawCurled(Leap::Finger finger) const {
  /*bool retVal = false;
  Leap::Bone metacarpal = finger.bone(Leap::Bone::TYPE_METACARPAL);
  Leap::Bone proximal = finger.bone(Leap::Bone::TYPE_PROXIMAL);
  Leap::Bone intermediate = finger.bone(Leap::Bone::TYPE_INTERMEDIATE);
  Leap::Bone distal = finger.bone(Leap::Bone::TYPE_DISTAL);
  
  float mToPDot = config::MAX_DOT_FOR_CONTINUE_POINTING;
  if ( finger.type() != Leap::Finger::TYPE_THUMB ) {
    mToPDot = metacarpal.direction().toVector3<Vector3>().dot(proximal.direction().toVector3<Vector3>());
  }
  float pToIDot = proximal.direction().toVector3<Vector3>().dot(intermediate.direction().toVector3<Vector3>());
  float iToDDot = intermediate.direction().toVector3<Vector3>().dot(distal.direction().toVector3<Vector3>());
  
  if (mToPDot >= config::MAX_DOT_FOR_CONTINUE_POINTING &&
      pToIDot <= config::MAX_DOT_FOR_CLAW &&
      pToIDot >= config::MIN_DOT_FOR_CLAW &&
      iToDDot <= config::MAX_DOT_FOR_CLAW &&
      iToDDot >= config::MIN_DOT_FOR_CLAW) {
    retVal = true;
  }
  return retVal;*/
  
  bool retVal = false;
  
  CircleFitter<3> fitter;
  
  for(int i=0; i < 4; i++ ) {
    Leap::Bone bone = finger.bone(static_cast<Leap::Bone::Type>(i));
    Vector3 position = bone.nextJoint().toVector3<Vector3>();
    fitter.AddPoint(position);
    //std::cout << i << ": " << position.transpose() << std::endl;
  }
  
  double error = fitter.Fit();
  error = std::sqrt(error);
  double radius = fitter.Radius();
  
  if ( finger.type() == Leap::Finger::TYPE_INDEX ) {
    std::cout << "error : " << error << std::endl;
    std::cout << "radius: " << radius << std::endl << std::endl;
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
