#pragma once
#include "ExposeControl.h"
#include "State.h"
#include "VolumeControl.h"

class AutoPacket;
struct RenderFrame;

class GraphicsStateMachine
{
public:
  GraphicsStateMachine();
  ~GraphicsStateMachine();

  State m_state;

  //Handle RenderFrames
  void AutoFilter(AutoPacket& packet, const RenderFrame& frame);
};

