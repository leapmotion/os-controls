#include "HandExistTrigger.h"

#include <algorithm>

HandExistTrigger::HandExistTrigger()
{
}

void HandExistTrigger::AutoFilter(const Leap::Frame& frame, HandExistenceState& heg){
  std::set<int32_t> hands;
  //std::cout << "test" << std::endl;
  for (auto hand : frame.hands()) {
    hands.insert(hand.id());
  }
  
  heg.m_goneHands.clear();
  
  std::vector<int32_t> newHands;
  std::set_difference(hands.begin(), hands.end(), m_hands.begin(), m_hands.end(), std::inserter(newHands,newHands.begin()));
  for(auto hand : newHands)
    heg.m_newHands.push_back(frame.hand(hand));
  
  std::vector<int32_t> stableHands;
  std::set_intersection(hands.begin(), hands.end(), m_hands.begin(), m_hands.end(), std::inserter(stableHands,stableHands.begin()));
  for(auto hand : stableHands)
    heg.m_stableHands.push_back(frame.hand(hand));
  
  std::vector<int32_t> goneHands;
  std::set_difference(m_hands.begin(), m_hands.end(), hands.begin(), hands.end(), std::inserter(goneHands,goneHands.begin()));
  for(auto hand : goneHands)
    heg.m_goneHands.push_back(frame.hand(hand));
  
  if(heg.m_newHands.size() > 0) {
    std::cout << "New Hand!!" << std::endl;
  }
  
  if(heg.m_goneHands.size() > 0) {
    std::cout << "Gone Hand :(" << std::endl;
  }
  
  m_hands = hands;
}