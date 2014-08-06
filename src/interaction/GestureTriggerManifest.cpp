#include "stdafx.h"
#include "GestureTriggerManifest.h"

#include "HandExistTrigger.h"

GestureTriggerManifest::GestureTriggerManifest()
{
  AutoRequired<HandExistTrigger>();
}

void GestureTriggerManifest::AutoFilter(Leap::Frame frame, const HandExistenceState& state, GestureTriggerManifestState& gtmfState) {
  
}