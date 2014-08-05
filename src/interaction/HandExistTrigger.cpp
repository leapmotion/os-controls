#include "HandExistTrigger.h"

#include <algorithm>

HandExistTrigger::HandExistGestures()
{
}


HandExistTrigger::~HandExistGestures()
{
}

void HandExistTrigger::AutoFilter(Leap::Frame frame){
  bool handExists = false;

  //TODO: Pull the actuall detection logic out into a bool-returning lambda function so we can
  //generalize the structure and reuse the detector lambdas
  std::set<int32_t> hands;

  for (auto hand : frame.hands()) {
    hands.insert(hand.id());
  }

  {
    std::set<int32_t> newHands;
    std::set_difference(hands.begin(), hands.end(), m_hands.begin(), m_hands.end(), newHands);

    for (auto hand : newHands) {
      m_eventTrigger(&HandExistGestureEvents::HandStart)(frame.hand(hand));
    }
  }

  {
    std::set<int32_t> sameHands;
    std::set_union(hands.begin(), hands.end(), m_hands.begin(), m_hands.end(), sameHands);

    for (auto hand : sameHands) {
      m_eventTrigger(&HandExistGestureEvents::HandUpdate)(frame.hand(hand));
    }
  }

  {
    std::set<int32_t> deadHands;
    std::set_difference(m_hands.begin(), m_hands.end(), hands.begin(), hands.end(), deadHands);

    for (auto hand : deadHands) {
      m_eventTrigger(&HandExistGestureEvents::HandEnd)(m_oldFrame.hand(hand));
    }
  }
}