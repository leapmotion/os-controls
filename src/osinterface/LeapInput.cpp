#include "stdafx.h"
#include "LeapInput.h"

LeapInput::LeapInput(void):
  ContextMember("LeapInput")
{
  m_controller->addListener(*this);
}

LeapInput::~LeapInput(void)
{
  m_controller->removeListener(*this);
}

void LeapInput::onFrame(const Leap::Controller& controller) {
  CurrentContextPusher pshr(this->GetContext());
  std::shared_ptr<AutoPacket> packet;
  try {
    packet = m_factory->NewPacket();
  }
  catch(autowiring_error&) {
    return;
  }
  
  controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);

  packet->Decorate(controller.frame());
}
