#include "HandExistTrigger.h"

#include <algorithm>

HandExistTrigger::HandExistTrigger()
{
}

void HandExistTrigger::AutoFilter(Leap::Frame frame, HandExistenceState& heg){
  bool handExists = false;

  //TODO: Pull the actuall detection logic out into a bool-returning lambda function so we can
  //generalize the structure and reuse the detector lambdas
  std::set<int32_t> hands;

  for (auto hand : frame.hands()) {
    hands.insert(hand.id());
  }

  std::vector<int32_t> newHands;
  std::set_difference(hands.begin(), hands.end(), m_hands.begin(), m_hands.end(), std::inserter(newHands,newHands.begin()));
  for(auto hand : newHands)
    heg.m_newHands.push_back(frame.hand(hand));
}