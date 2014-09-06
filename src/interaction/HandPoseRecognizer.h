#pragma once

#include "Leap.h"
#include "EigenTypes.h"
#include "HandPinchRecognizer.h"
#include <Eigen/Dense>
#include <map>

#include <autowiring/Autowired.h>

enum class HandPose {
  ZeroFingers,
  OneFinger,
  TwoFingers,
  ThreeFingers,
  FourFingers,
  FiveFingers,
  Clawed
};

class HandPoseRecognizer {
public:
  HandPoseRecognizer(void);

  void AutoFilter(const Leap::Hand& frame, const FrameTime& frameTime, const HandPinch& handPinch, HandPose& handPose);
private:
  const float activate_clawCurl_min = 0.15f;
  const float activate_clawCurl_max = 1.0f;
  const float persist_clawCurl_min = 0.15f;
  const float persist_clawCurl_max = 1.5f;
  const float activate_distance = 10.0f;
  const float persist_distance = 0.0f;
  const float activate_palmDown = -0.95f;
  const float persist_palmDown = -1.0f;
  const float activate_fingersForward = 60.0f;
  const float persist_fingersForward = 50.0f;
  const float activate_pinchVelocity = 0.5;
  const float persist_pinchVelocity = 100.0f; //essentially anything
  const float activate_fingerVelocity = 0.2f;
  const float persist_fingerVelocity = 100.0f; //essentially anything
  
  bool isExtended(Leap::Finger finger, bool wasExtended = false) const;
  bool isClawCurled(Leap::Finger finger, float curlMin, float curlMax) const;
  bool areTipsSeparated(Leap::Hand hand, float thresholdDistance) const;
  bool lastExtended [5];
  Eigen::Matrix<double,3,5> lastPosition;
  //Vector3 lastPosition [5];
  float averageFingerBend(Leap::Finger finger) const;
  bool isDown(Leap::Finger finger) const;
  float projectAlongPalmNormal(Vector3 point, Leap::Hand hand) const;
  
  HandPose m_lastPose;
};