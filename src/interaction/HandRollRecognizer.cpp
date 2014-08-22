#include "stdafx.h"
#include "HandRollRecognizer.h"

HandRollRecognizer::HandRollRecognizer(void):
  m_hasLast(false),
  m_lastRoll(0.0)
{
}

HandRollRecognizer::~HandRollRecognizer(void)
{
}

void HandRollRecognizer::AutoFilter(const Leap::Hand& hand, DeltaRollAmount& dra) {
  // Compute the roll amount, decide whether to floor it down to zero
  float roll = -hand.palmNormal().roll();
  if(std::abs(roll - m_lastRoll) > 0.5f)
    // Roll is above the threshold, set the value
    dra.dTheta = roll - m_lastRoll;
  else
    // Below the threshold, zeroize
    dra.dTheta = 0.0;

  std::cout << "Roll: " << roll << std::endl;
  std::cout << "dRoll: " << roll - m_lastRoll << std::endl;

  // Need to keep track of what the last roll was, now
  m_lastRoll = roll;

  // Zeroize theta if we don't have a prior roll value
  if(!m_hasLast) {
    dra.dTheta = 0.0;
    m_hasLast = true;
  }
}