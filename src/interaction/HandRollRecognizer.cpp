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
  
  dra.dTheta = roll - m_lastRoll;
  
  // Need to keep track of what the last roll was, now
  m_lastRoll = roll;

  // Zeroize theta if we don't have a prior roll value
  if(!m_hasLast) {
    dra.dTheta = 0.0;
    m_hasLast = true;
  }
}