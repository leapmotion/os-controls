#include "stdafx.h"
#include "GestureTriggerManifest.h"

#include "HandExistTrigger.h"
#include "HandLocationRecognizer.h"
#include "CircleGestureTrigger.h"
#include "HandPoseRecognizer.h"

GestureTriggerManifest::GestureTriggerManifest()
{
  //List all gesture triggers here so that they will be created in the correct context

  AutoRequired<HandExistTrigger>();
  AutoRequired<HandLocationRecognizer>();
  AutoRequired<CircleGestureTrigger>();
  AutoRequired<HandPoseRecognizer>();
}
