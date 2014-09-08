#include "stdafx.h"
#include "HandDataCombiner.h"

HandDataCombiner::HandDataCombiner() { }
HandDataCombiner::~HandDataCombiner() { }

void HandDataCombiner::AutoFilter(const HandLocation &handLocation, const HandTime& handTime, const HandPose &handPose, const HandRoll &handRoll, const HandPinch &handPinch, HandData &handData) {
  handData.locationData = handLocation;
  handData.handPose = handPose;
  handData.rollData = handRoll;
  handData.pinchData = handPinch;
  handData.timeVisible = handTime.timeVisible;
}