#include "stdafx.h"
#include "StateMachineContextManifest.h"

#include <autowiring/AutoPacketFactory.h>
#include "StateMachine.h"
#include "HandDataCombiner.h"
#include "ScrollRecognizer.h"

#include "cursor/CursorView.h"

#include "mediaview/MediaViewController.h"
#include "mediaview/MediaViewStateMachine.h"

#include "expose/ExposeActivationStateMachine.h"
#include "expose/ExposeViewStateMachine.h"

StateMachineContextManifest::StateMachineContextManifest()
{
  AutoRequired<AutoPacketFactory>();

  AutoRequired<StateMachine>();

  // HandDataCombiner will introduce additional depedent types
  AutoRequired<HandDataCombiner>();

  // TODO: Merge this into HandDataCombiner
  AutoRequired<ScrollRecognizer>();

  AutoRequired<CursorView> m_cursorView;

  AutoRequired<MediaViewController> m_mediaViewController;
  AutoRequired<MediaViewStateMachine> m_mediaViewStateMachine;

  AutoRequired<ExposeActivationStateMachine> m_exposeActivationStateMachine;
  AutoRequired<ExposeViewStateMachine> m_evp;
}
