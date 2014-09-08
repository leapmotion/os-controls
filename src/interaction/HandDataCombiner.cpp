#include "stdafx.h"
#include "HandDataCombiner.h"

HandDataCombiner::HandDataCombiner() { }
HandDataCombiner::~HandDataCombiner() { }

void HandDataCombiner::AutoFilter(const HandLocation &handLocation, const HandTime& handTime, const HandPose &handPose, const HandRoll &handRoll, const HandPinch &handPinch, HandData &handData) {
  handData.handLocation = handLocation;
  handData.handPose = handPose;
  handData.handRoll = handRoll;
  handData.handPinch = handPinch;
}