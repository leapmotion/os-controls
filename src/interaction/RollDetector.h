#pragma once
#include <Leap.h>

/// <summary>
/// Refers to a noise-filtered roll amount by the user
/// </summary>
struct DeltaRollAmount {
  // The amount of the roll, in radians
  float dTheta;
};

/// <summary>
/// A simplified thresholded roll detector
/// </summary>
class RollDetector
{
public:
  RollDetector(void);
  ~RollDetector(void);

  void AutoFilter(const Leap::Hand& hand, DeltaRollAmount& dra);

private:
  // True if no packets have been received yet--used to guard against initialization spikes
  bool m_hasLast;

  // Last measured hand roll amount, used to guard against wander
  float m_lastRoll;
};

