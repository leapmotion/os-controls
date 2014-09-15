#include "stdafx.h"
#include "FrameFragmenter.h"
#include "GestureTriggerManifest.h"
#include "HandEventListener.h"
#include "StateMachine.h"
#include "StateMachineContext.h"
#include <Leap.h>

FrameFragmenter::FrameFragmenter(void) :
m_activeHandID(Leap::Hand::invalid().id())
{
}

FrameFragmenter::~FrameFragmenter(void)
{
}

std::shared_ptr<CoreContext> FrameFragmenter::CreateMenuContext(const Leap::Hand& hand) const {
  AutoCreateContextT<StateMachineContext> ctxt;
  CurrentContextPusher pshr(ctxt);

  // Stick the things in the context that we need in the context
  AutoRequired<StateMachine>();
  AutoRequired<AutoPacketFactory>();
  GestureTriggerManifest();

  // Done setting it up, initiate it and hand it back
  ctxt->Initiate();
  return ctxt;
}

void FrameFragmenter::OnLeapFrame(const Leap::Frame& frame) {
  // Hold on to our contexts, and then feed them back into m_contexts as we encounter them
  std::unordered_map<int, std::shared_ptr<CoreContext>> contexts;
  std::swap(contexts, m_contexts);

  //Update the active hand ID - search in the current frame, grab a new one if the old one is gone
  m_activeHandID = frame.hand(m_activeHandID).id();
  auto hands = frame.hands();
  if (m_activeHandID == Leap::Hand::invalid().id() && hands.count() > 0) {
    m_activeHandID = hands[0].id();
  }

  const auto& hand = frame.hand(m_activeHandID);
  if (hand != Leap::Hand::invalid()) {
    std::shared_ptr<CoreContext>& ctxt = contexts[hand.id()];

    if(!ctxt)
      // Need to initialize a new context, spin it up and send it off
      ctxt = CreateMenuContext(hand);

    // Decorate with a pointer to the hand.  If a decision must be made about whether to drive this
    // context, it must be made at this point.
    AutoRequired<AutoPacketFactory> factory(ctxt);
    auto packet = factory->NewPacket();
    packet->Decorate(frame);
    packet->Decorate(&frame);
    packet->Decorate(hand);
    packet->Decorate(&hand);

    // We found this context this time, do an implicit set difference by moving it
    // into our known set of subcontexts:
    m_contexts[hand.id()] = ctxt;
    contexts.erase(hand.id());
  }

  // Tell each orphan context that we've got no further information for them.  Then, when this
  // loop exits, it will be the responsibility of these subcontexts to decide when they go away.
  for(auto& value : contexts) {
    AutoFired<HandEventListener> hel(value.second);
    hel(&HandEventListener::OnHandVanished)();
  }
}
