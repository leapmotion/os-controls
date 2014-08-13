#include "stdafx.h"
#include "GraphicsStateMachine.h"
#include <autowiring/autowiring.h>
#include "RenderState.h"

GraphicsStateMachine::GraphicsStateMachine() : m_state(State::MediaView)
{
}

GraphicsStateMachine::~GraphicsStateMachine()
{
}

void GraphicsStateMachine::AutoFilter(AutoPacket& packet, const RenderFrame& scene) {
  switch(m_state) {
  case State::MediaView:
    packet.Decorate(StateSentry<State::MediaView>());
    break;
  case State::ExposeMaker:
    packet.Decorate(StateSentry<State::ExposeMaker>());
    break;
  case State::FunnyThirdThing:
    packet.Decorate(StateSentry<State::FunnyThirdThing>());
    break;
  }
}
