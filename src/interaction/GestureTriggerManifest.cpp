#include "stdafx.h"
#include "GestureTriggerManifest.h"

#include "HandExistTrigger.h"

GestureTriggerManifest::GestureTriggerManifest()
{
  AutoRequired<HandExistTrigger>();
}