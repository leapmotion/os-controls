#pragma once

#include "Leap.h"

#include <autowiring/Autowired.h>

struct SystemWipe {
  bool isWiping;
};

class SystemWipeRecognizer {
public:

  void AutoFilter(const Leap::Frame& frame, SystemWipe& systemWipe);
};
