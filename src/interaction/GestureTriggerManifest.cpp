#include "stdafx.h"
#include "GestureTriggerManifest.h"

#include "HandExistTrigger.h"
#include "HandCoordinatesDecorator.h"
#include "HandPointingDecorator.h"

GestureTriggerManifest::GestureTriggerManifest()
{
  //List all gesture triggers here so that they will be created in the correct context

  AutoRequired<HandExistTrigger>();
  AutoRequired<HandCoordinatesDecorator>();
  AutoRequired<HandPointingDecorator>();
}
