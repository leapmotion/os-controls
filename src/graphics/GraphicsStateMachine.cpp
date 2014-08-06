#include "stdafx.h"
#include "GraphicsStateMachine.h"
#include <autowiring/autowiring.h>

GraphicsStateMachine::GraphicsStateMachine()
{
}

GraphicsStateMachine::~GraphicsStateMachine()
{
}

void GraphicsStateMachine::AutoFilter(AutoPacket& packet, const Scene& scene) {
  switch(m_state) {
  case State::VolumeControl:
    packet.Decorate(StateSentry<State::VolumeControl>());
    break;
  case State::ExposeMaker:
    packet.Decorate(StateSentry<State::ExposeMaker>());
    break;
  case State::FunnyThirdThing:
    packet.Decorate(StateSentry<State::FunnyThirdThing>());
    break;
  }
}
