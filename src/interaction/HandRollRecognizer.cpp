#include "stdafx.h"
#include "HandRollRecognizer.h"

HandRollRecognizer::HandRollRecognizer(void):
  m_hasLast(false),
  m_lastRoll(0.0)
{
  m_deltaRoll.SetInitialValue(0.0f);
  m_deltaRoll.SetSmoothStrength(0.99f);
}

HandRollRecognizer::~HandRollRecognizer(void)
{
}

void HandRollRecognizer::AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, DeltaRollAmount& dra) {
  // Compute the roll amount, decide whether to floor it down to zero
  float roll = -hand.palmNormal().roll();
  
  dra.dTheta = roll - m_lastRoll;
  m_deltaRoll.SetGoal(roll - m_lastRoll);
  
  m_deltaRoll.Update(frameTime.deltaTime);
  
  // Need to keep track of what the last roll was, now
  m_lastRoll = roll;

  // Zeroize theta if we don't have a prior roll value
  if(!m_hasLast) {
    m_deltaRoll.SetInitialValue(0.0);
    m_hasLast = true;
  }
  
  dra.dTheta = m_deltaRoll.Value();
  
  dra.absoluteRoll = roll;
}