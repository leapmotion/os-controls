#pragma once
#include "Leap.h"
#include "HandPoseRecognizer.h"
#include "uievents/HandProperties.h"
#include <autowiring/Autowired.h>
#include <map>

class CoordinateUtility;

class HandLocationRecognizer {
public:
  HandLocationRecognizer(void);
  ~HandLocationRecognizer(void);

  void AutoFilter(const Leap::Hand& hand, const HandPose& handPose, HandLocation& handLocation);

private:
  AutoRequired<CoordinateUtility> m_coordinateUtility;
};