#pragma once
#include "osinterface/OSWindow.h"

class ExposeViewEvents {
public:
  virtual~ExposeViewEvents(void);
  
  virtual void onSelectionMade(std::shared_ptr<OSWindow>) {};
};