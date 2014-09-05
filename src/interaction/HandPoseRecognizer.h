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

  void AutoFilter(const Leap::Hand& frame, HandPose& handPose);
private:
  const float activate_clawCurl_min = 0.25f;
  const float activate_clawCurl_max = 2.0f;
  const float persist_clawCurl_min = 0.06f;
  const float persist_clawCurl_max = 2.5f;
  const float activate_distance = 10.0f;
  const float persist_distance = 1.0f;
  const float activate_palmDown = -0.9f;
  const float persist_palmDown = -1.0f;
  const float activate_fingersForward = 50.0f;
  const float persist_fingersForward = 40.0f;
  
  bool isExtended(Leap::Finger finger, bool wasExtended = false) const;
  bool isClawCurled(Leap::Finger finger) const;
  bool areTipsSeparated(Leap::Hand hand) const;
  bool lastExtended [5];
  float averageFingerBend(Leap::Finger finger) const;
  bool isDown(Leap::Finger finger) const;
  float projectAlongPalmNormal(Vector3 point, Leap::Hand hand) const;
  
  HandPose m_lastPose;
};