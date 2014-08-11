#pragma once
#include "ExposeControl.h"
#include "State.h"
#include "VolumeControl.h"

class AutoPacket;

class GraphicsStateMachine
{
public:
  GraphicsStateMachine();
  ~GraphicsStateMachine();

  State m_state;

  void AutoFilter(AutoPacket& packet, const Scene& scene);
};

