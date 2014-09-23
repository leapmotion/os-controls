#include "stdafx.h"
#include "LeapInput.h"
#include "LeapInputListener.h"
#include "OSVirtualScreen.h"
#include "interaction/FrameFragmenter.h"

LeapInput::LeapInput(void):
  ContextMember("LeapInput"),
  m_isAcceptingInput(false)
{
  m_controller->addListener(*this);
}

LeapInput::~LeapInput(void)
{
  m_controller->removeListener(*this);
}

bool LeapInput::AcceptInput(void) const {
  // Ignore input when the screen saver is running
  return m_virtualScreen && !m_virtualScreen->IsScreenSaverActive();
}

void LeapInput::AbortInput(void) {
  m_isAcceptingInput = false;
  CurrentContextPusher pshr(this->GetContext());
  // Send an invalid frame to abort any interactions using the Leap input
  m_listener(&LeapInputListener::OnLeapFrame)(Leap::Frame::invalid());
}

void LeapInput::onServiceConnect(const Leap::Controller& controller) {
  controller.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
}

void LeapInput::onDisconnect(const Leap::Controller& controller) {
  AbortInput();
}

void LeapInput::onFocusLost(const Leap::Controller& controller) {
  AbortInput();
}

void LeapInput::onFrame(const Leap::Controller& controller) {
  if (!AcceptInput()) {
    if (m_isAcceptingInput) {
      AbortInput();
    }
    return;
  }
  m_isAcceptingInput = true;
  CurrentContextPusher pshr(this->GetContext());
  m_listener(&LeapInputListener::OnLeapFrame)(controller.frame());
}
