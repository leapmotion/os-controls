#include "stdafx.h"
#include "HandDataCombiner.h"

HandDataCombiner::HandDataCombiner() { }
HandDataCombiner::~HandDataCombiner() { }

void HandDataCombiner::AutoFilter(const HandLocation &handLocation, const HandTime& handTime, const HandPose &handPose, const HandRoll &handRoll, const HandPinch &handPinch, const HandGrab& handGrab, HandData &handData) {
  handData.locationData = handLocation;
  handData.handPose = handPose;
  handData.rollData = handRoll;
  handData.pinchData = handPinch;
  handData.grabData = handGrab;
  handData.timeVisible = handTime.timeVisible;
}