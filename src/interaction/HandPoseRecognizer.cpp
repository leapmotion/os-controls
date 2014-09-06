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
  
  //Initialize Claw Check Flags
  bool isClawCurling = true;
  bool isClawDistance = false;
  bool isPalmPointingDown = false;
  bool fingersOut = true;
  bool pinchIsSteady = true;
  //bool fingerTipsSteady = true;
  bool isPoseCorrect = false;
  
  bool isThumExtended = false;
  bool isIndexExtended = false;
  bool isMiddleExtended = true;
  bool areRingAndPinkeyExtended = false;
  
  int handCode = 0; // for extention based pose recognition.
  
  int i = 0; // index tracker for for loop.
  
  float averageVelocity = 0.0f;
  float velocitySum = 0.0f; // sum of finger tip velocities
  
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
    
    float positionDiffDistance = (finger.tipPosition().toVector3<Vector3>() - lastPosition.col(i)).norm();
    float velocity = positionDiffDistance / (frameTime.deltaTime / 1000.0);
    velocitySum += velocity;
    lastPosition.col(i) = finger.tipPosition().toVector3<Vector3>();
    
    // Per finger checks for claw pose
    if ( finger.type() == Leap::Finger::TYPE_INDEX ||
         finger.type() == Leap::Finger::TYPE_MIDDLE ) {
      
      Vector3 palmPos = hand.palmPosition().toVector3<Vector3>();
      Vector3 tipPos = finger.tipPosition().toVector3<Vector3>();
      Vector3 diff = tipPos - palmPos;
      float zDist = fabs(diff.z());
      
      if ( isExtended(finger)) {
        if ( finger.type() == Leap::Finger::TYPE_INDEX ) {
          isIndexExtended = true;
        }
        else {
          isMiddleExtended = true;
        }
      }
      
      switch ( m_lastPose ) {
        case HandPose::Clawed:
          if ( zDist < persist_fingersForward ) {
            fingersOut = false;
          }
          
          isClawCurling = isClawCurled(finger, persist_clawCurl_min, persist_clawCurl_max);
          break;
          
        default:
          if ( zDist < activate_fingersForward ) {
            fingersOut = false;
          }
          
          isClawCurling = isClawCurled(finger, activate_clawCurl_min, activate_clawCurl_max);
          break;
      }
      
    }
    else if ( finger.type() == Leap::Finger::TYPE_THUMB ) {
      if ( isExtended(finger) )
      {
        isThumExtended = true;
      }
    }
    else {
      if ( finger.isExtended() ) {  
        areRingAndPinkeyExtended = true;
      }
    }
    

    i++;
  }
  
  averageVelocity = velocitySum / 5.0f;
  
  // Whole hand checks for claw pose
  float palmY = hand.palmNormal().toVector3<Vector3>().y();
  switch ( m_lastPose ) {
    case HandPose::Clawed:
      
      if ( (isMiddleExtended || isThumExtended) &&
           !areRingAndPinkeyExtended) {
        isPoseCorrect = true;
      }
      
      if ( palmY < persist_palmDown ) {
        isPalmPointingDown = true;
      }
      
      isClawDistance = areTipsSeparated(hand, persist_distance);
      
      if ( fabs(handPinch.pinchDeltaPerSecond) > persist_pinchVelocity ) { pinchIsSteady = false; }
      //if ( averageVelocity > persist_fingerVelocity ) { fingerTipsSteady = false; }
      break;
    default:
      
      if ( isIndexExtended &&
           isMiddleExtended &&
           isThumExtended &&
           !areRingAndPinkeyExtended) {
        isPoseCorrect = true;
      }
      
      if ( palmY <= activate_palmDown ) {
        isPalmPointingDown = true;
      }
      
      isClawDistance = areTipsSeparated(hand, activate_distance);
      
      if ( fabs(handPinch.pinchDeltaPerSecond) > activate_pinchVelocity ) { pinchIsSteady = false; }
      //if ( averageVelocity > activate_fingerVelocity ) { fingerTipsSteady = false; }
      break;
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
  
  
  //CLAW POSE DEBUG SET
  /*
  std::cout << "isIndexExtended: " << isIndexExtended << std::endl;
  std::cout << "isMiddleExtended: " << isIndexExtended << std::endl;
  std::cout << "isThumExtended: " << isIndexExtended << std::endl;
  std::cout << "areRingAndPinkeyExtended: " << isIndexExtended << std::endl;
  std::cout << "isPoseCorrect: " << isPoseCorrect << std::endl;
  std::cout << "isClawCurling: " << isClawCurling << std::endl;
  std::cout << "isClawDistance: " << isClawDistance << std::endl;
  std::cout << "fingersOut: " << fingersOut << std::endl;
  std::cout << std::endl;
   */
  
  // Claw-Factor based pose resolution
  if (isPoseCorrect &&
      isClawCurling &&
      isClawDistance &&
      fingersOut) {
    handPose = HandPose::Clawed;
  }
  
  //Store the previous hand pose.
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

bool HandPoseRecognizer::isClawCurled(Leap::Finger finger, float curlMin, float curlMax) const {
  bool retVal = false;
  float averageBend = averageFingerBend(finger);
  
  if ( averageBend > curlMin && averageBend < curlMax ) {
    retVal = true;
  }
  
  return retVal;
}

bool HandPoseRecognizer::areTipsSeparated(Leap::Hand hand, float thresholdDistance) const{
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
    if ( distance <  thresholdDistance) {
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
  
  int startBone = 3;
  
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
