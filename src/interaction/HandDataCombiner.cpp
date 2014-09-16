#include "stdafx.h"
#include "HandDataCombiner.h"
#include <autowiring/AutoPacketFactory.h>


HandDataCombiner::HandDataCombiner() {
  AutoRequired<HandLocationRecognizer>();
  AutoRequired<TimeRecognizer>();
  AutoRequired<HandPoseRecognizer>();
  AutoRequired<HandRollRecognizer>();
  AutoRequired<HandActivationRecognizer>();
  AutoRequired<ScrollRecognizer>();
}
HandDataCombiner::~HandDataCombiner() { }

void HandDataCombiner::AutoFilter(const HandLocation &handLocation, const HandPose &handPose, const HandRoll &handRoll, const HandPinch &handPinch, const HandGrab& handGrab, const Scroll& handScroll, const HandTime& handTime, HandData &handData) {
  handData.locationData = handLocation;
  handData.handPose = handPose;
  handData.rollData = handRoll;
  handData.pinchData = handPinch;
  handData.grabData = handGrab;
  handData.scroll = handScroll;
  handData.timeVisible = handTime.timeVisible;
}
