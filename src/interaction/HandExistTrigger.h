#pragma once
#include <autowiring/Autowired.h>
#include <set>

#include "Leap.h"

class HandExistGestureEvents 
{
public:
  virtual void HandStart(Leap::Hand) {}
  virtual void HandEnd(Leap::Hand) {}
  virtual void HandUpdate(Leap::Hand) {}
};

class HandExistTrigger:
  public ContextMember
{
public:
  HandExistTrigger();
  virtual ~HandExistTrigger() {};

  void AutoFilter(Leap::Frame frame);

private:
  std::set<int32_t> m_hands;
  Leap::Frame m_oldFrame;

  AutoFired<HandExistGestureEvents> m_eventTrigger;
};

