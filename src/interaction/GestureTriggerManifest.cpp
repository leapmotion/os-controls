#include "stdafx.h"
#include "GestureTriggerManifest.h"

#include "HandLocationRecognizer.h"
#include "HandPoseRecognizer.h"
#include "HandRollRecognizer.h"
#include "ClawRotationRecognizer.h"
#include "FrameDeltaTimeRecognizer.h"
#include "HandPinchRecognizer.h"
#include "ScrollRecognizer.h"
#include "StateMachine.h"

GestureTriggerManifest::GestureTriggerManifest()
{
  //List all gesture triggers here so that they will be created in the correct context

  AutoRequired<HandLocationRecognizer>();
  AutoRequired<HandPoseRecognizer>();
  AutoRequired<HandRollRecognizer>();
  AutoRequired<FrameDeltaTimeRecognizer>();
  AutoRequired<HandPinchRecognizer>();
  AutoRequired<StateMachine>();
  AutoRequired<ScrollRecognizer>();
}
