#pragma once

#include<autowiring/Autowired.h>
#include "Leap.h"

class GestureRecognizer :
  public ContextMember
{
public:
  GestureRecognizer();
  virtual ~GestureRecognizer() {};

  void AutoFilter(Leap::Frame frame);

private:

};