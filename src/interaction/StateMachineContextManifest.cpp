#include "stdafx.h"
#include "StateMachineContextManifest.h"

#include <autowiring/AutoPacketFactory.h>
#include "StateMachine.h"
#include "uievents/ShortcutsDomain.h"
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
  AutoRequired<AutoSelfUpdate<ShortcutsStateClass>>();

  // HandDataCombiner will introduce additional depedent types
  AutoRequired<HandDataCombiner>();

  AutoRequired<CursorView>();

  AutoRequired<MediaViewController>();
  AutoRequired<MediaViewStateMachine>();

  AutoRequired<ExposeActivationStateMachine>();
  AutoRequired<ExposeViewStateMachine>();
}
