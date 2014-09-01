#include "stdafx.h"
#include "LeapInput.h"
#include "LeapInputListener.h"
#include "interaction/FrameFragmenter.h"

LeapInput::LeapInput(void):
  ContextMember("LeapInput")
{
  m_controller->enableGesture(Leap::Gesture::TYPE_CIRCLE);
  m_controller->setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
  m_controller->addListener(*this);
}

LeapInput::~LeapInput(void)
{
  m_controller->removeListener(*this);
}

void LeapInput::onFrame(const Leap::Controller& controller) {
  CurrentContextPusher pshr(this->GetContext());
  m_listener(&LeapInputListener::OnLeapFrame)(controller.frame());
}
