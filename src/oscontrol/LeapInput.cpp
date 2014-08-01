#include "stdafx.h"
#include "LeapInput.h"

LeapInput::LeapInput(void)
{
  m_controller->addListener(*this);
}

LeapInput::~LeapInput(void)
{
  m_controller->removeListener(*this);
}

void LeapInput::onFrame(const Leap::Controller& controller) {
  std::shared_ptr<AutoPacket> packet;
  try {
    packet = m_factory->NewPacket();
  }
  catch(autowiring_error&) {
    return;
  }

  packet->Decorate(controller.frame());
}
