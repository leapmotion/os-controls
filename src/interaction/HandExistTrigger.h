#pragma once
#include <autowiring/Autowired.h>
#include <set>

#include "Leap.h"

struct HandExistenceState {
  std::vector<Leap::Hand> m_newHands;
  std::vector<Leap::Hand> m_stableHands;
  std::vector<Leap::Hand> m_goneHands;
};

class HandExistTrigger:
  public ContextMember
{
public:
  HandExistTrigger();
  virtual ~HandExistTrigger() {};

  void AutoFilter(AutoPacket& packet, Leap::Frame frame, HandExistenceState& heg);

private:
  std::set<int32_t> m_hands;
  Leap::Frame m_oldFrame;
};
