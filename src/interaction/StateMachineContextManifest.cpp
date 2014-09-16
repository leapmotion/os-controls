#include "stdafx.h"
#include "StateMachineContextManifest.h"

#include <autowiring/AutoPacketFactory.h>
#include "StateMachine.h"
#include "uievents/OSCDomain.h"
#include <autowiring/AutoSelfUpdate.h>

#include "HandDataCombiner.h"

#include "cursor/CursorView.h"

#include "mediaview/MediaViewController.h"
#include "mediaview/MediaViewStateMachine.h"

#include "expose/ExposeActivationStateMachine.h"
#include "expose/ExposeViewStateMachine.h"

StateMachineContextManifest::StateMachineContextManifest()
{
  AutoRequired<AutoPacketFactory>();

  AutoRequired<StateMachine>();
  AutoRequired<AutoSelfUpdate<OSCStateClass>>();

  // HandDataCombiner will introduce additional depedent types
  AutoRequired<HandDataCombiner>();

  AutoRequired<CursorView> m_cursorView;

  AutoRequired<MediaViewController> m_mediaViewController;
  AutoRequired<MediaViewStateMachine> m_mediaViewStateMachine;

  AutoRequired<ExposeActivationStateMachine> m_exposeActivationStateMachine;
  AutoRequired<ExposeViewStateMachine> m_evp;
}
