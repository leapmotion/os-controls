#pragma once
#include "osinterface/LeapInputListener.h"

class ExposeView;

class TestStateMachine:
  public LeapInputListener
{
public:
  TestStateMachine();
  ~TestStateMachine();

private:
  Autowired<ExposeView> m_view;

public:
  void OnLeapFrame(const Leap::Frame& frame) override;
};

