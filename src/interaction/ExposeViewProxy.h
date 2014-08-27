#pragma once
#include "uievents/OSCDomain.h"

class ExposeViewAccessManager;

class ExposeViewProxy
{
public:
  ExposeViewProxy();
  ~ExposeViewProxy();

  void AutoFilter(OSCState state);

private:
  Autowired<ExposeViewAccessManager> evam;
};

