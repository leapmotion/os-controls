//
//  HandCoordinatesDecorator.cpp
//  oscontrols
//
//  Created by Daniel Plemmons on 8/15/14.
//
//

#include "HandRollDecorator.h"
#include "HandPointingDecorator.h"
#include "CoordinateUtility.h"
#include "InteractionConfigs.h"

void HandRollDecorator::AutoFilter(const Leap::Frame& frame, const HandPoseVector& hpv, RollMap& handRolls) {
  for(auto hand : frame.hands()) {
    try {
      hpv[1].at(hand.id());
      handRolls[hand.id()] = -hand.palmNormal().roll();
    }
    catch (std::out_of_range e){
      //don't worry about it.
    }
  }
  //Creates a handScreenLocations reference and decorates the packet at the end of the packet.
}