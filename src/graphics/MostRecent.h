#pragma once

#include "interaction/HandExistTrigger.h"

class MostRecent {
public:
  HandExistenceState m_mostRecent;
  
  void AutoFilter(const HandExistenceState& hes) {
    m_mostRecent = hes;
  };
};

//#include <autowiring/AutoSelfUpdate.h>

//State information prior<HandExistenceState> will be decorated
//on every packet when the packet is created
//typedef AutoSelfUpdate<HandExistenceState> SharedMostRecent;*/